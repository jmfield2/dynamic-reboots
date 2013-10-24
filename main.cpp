#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iomanip>

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <vector>

using namespace std;

#define DEBUG 1

//GLOBAL VARIABLES
const string inputFile = "input.txt";
const string outputFile = "cook.txt";
long positionR; // IGNORE - file position holder.

// A day is defined as x(i) and s(i) from the input.  x(i) = data available on day i.  s(i) = processing ability on day i.
struct day {
    int dataAvailable;  //x(i)
    int dataAbility;    //s(i)
};

//FUNCTION PROTOTYPES
void PrepOutputFile();   //create and clear output from previous execution
int GetNumSets();   //read in total number of serpate data-sets to process
int GetInput(vector<day> & daysList);  //read in n 'days' from file into vector of type 'days'.
void CalculateTables(vector<day> &daysList, vector< vector<int> > &Costs, vector< vector<bool> > &Choice, int numDays, int &numReboots);
void WriteOutput(vector< vector<int> > &Costs, vector< vector<bool> > &Choice, int numReboots, int numDays);


int main()
{
    PrepOutputFile();
    int numSets = GetNumSets(); //total number datasets
    
    for(int i = 0; i < numSets; i++)    //MAIN LOOP FOR EACH DATA-SET
    {
        vector<day> daysList;   //Vector to hold X(i) and S(i) from input file.
        int numDays = GetInput(daysList);   //read in input from file to daysList.  Return # elements (days) in the current dataset.
        int numReboots = 0;
        
        vector< vector<int> > Costs(numDays, vector<int>(numDays, 0));        //dynamic programming aspect which holds previously calculated values.
        vector< vector<bool> > Choice(numDays, vector<bool>(numDays, false)); //dynamic programming aspect which holds each (continue/reboot) choice.
        
        CalculateTables(daysList, Costs, Choice, numDays, numReboots);
        WriteOutput(Costs, Choice, numReboots, numDays);
    }
    return 0;
}


void PrepOutputFile()
{
    ofstream outFile;   //clear output file
    outFile.open(outputFile.data());
    outFile.close();
}

int GetNumSets()
{
    int numSets = 0;
    ifstream inFile;    //get num sets
    inFile.open(inputFile.data());
    if(inFile.is_open())
    {
        inFile >> numSets;
        positionR = inFile.tellg();
    }
    inFile.close();
    return numSets;
}

int GetInput(vector<day>& daysList)
{
    int numDays = 0;
    fstream inFile;
    inFile.open(inputFile.data());
    
    if(inFile.is_open())
    {
        inFile.seekg(positionR);
        inFile >> numDays;
        for(int i = 0; i < numDays; i++)
        {
            day temp;
            inFile >> temp.dataAvailable;
            daysList.push_back(temp);
        }
        
        for(int i = 0; i < numDays; i++)
        {
            inFile >> daysList[i].dataAbility;
        }
        
    }
    positionR = inFile.tellg();
    inFile.close();
    return numDays;
}

void CalculateTables(vector<day> &daysList, vector< vector<int> > &Costs, vector< vector<bool> > &Choice, int numDays, int &numReboots)
{

    //Fill in table for last column.  We logically will never choose to reboot on the last day.
    for(int i = 0; i < numDays; i++)
    {
        int lastDay = numDays-1;    //bounds change; array starts at 0, not 1
        int lhs = daysList[lastDay].dataAvailable;  //
        int rhs = daysList[i].dataAbility;
        int minCost = min(lhs, rhs);
        
        Costs[lastDay][i] = minCost;
        Choice[lastDay][i] = 1; //no reason to reboot on last day.
    }

    // Fill in table for rest of columns.
    //
    // Table will have the SUM cost of i+(i+1) for a given column
    for(int i = (numDays-2); i >= 0; i--)
    {

	// Starting from day before last day, iterate each day from last to first
	// Then, iterate from 0th day up to ith day since reboot
        for(int j = 0; j <= i; j++)
        {
	    // Set left-hand to yesterday's max (0th day from reboot)
            int lhs = Costs[i+1][0];

	    // Set right-hand to min(available, able) + yesterday sum (i+1 day, j+1 since reboot ... the diagonal)
            int rhs = min(daysList[i].dataAvailable, daysList[j].dataAbility) + Costs[i+1][j+1] ;
            int maximum = max(lhs, rhs);

            Costs[i][j] = maximum;

            if(maximum == lhs)
            {
                Choice[i][j] = 0; //reboot
                if(j == 0)
                    numReboots++;
            }
            else
            {
                Choice[i][j] = 1; //process data
            }
 
        }

    }

#ifdef DEBUG
for (int i=0; i < numDays; i++) {
	for (int j=0; j < numDays; j++) 
		cout << setw(5) << Costs[j][i] << "-" << Choice[j][i] << " ";
	cout << endl;
}
#endif

}

void WriteOutput(vector< vector<int> > &Costs, vector< vector<bool> > &Choice, int numReboots, int numDays)
{
    vector<int> rebootDays;

    ofstream outFile;
    outFile.open(outputFile.data(), ios::app);
    if(outFile.is_open())
    {
	/* 
	 Traverse CHOICE n*n vector in a diagonal looking for
	 the days we reboot...if found, go back to 0 and continue
	 */

        int j = 0;
        outFile << Costs[0][0] << endl;
        for(int i = 0; i < numDays; i++)
        {
            if(Choice[i][j] == 0)
            {
                rebootDays.push_back(i+1);
                j = 0;
            }
            else{j++;}
        }

        outFile << rebootDays.size() << endl;

        for(int i = 0; i < rebootDays.size(); i++)
        {
            outFile << rebootDays[i] << " ";
        }

        outFile << "\n\n";
    }
    outFile.close();
}


