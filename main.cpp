
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;


//GLOBAL VARIABLES
string inputFile = "/Users/Steve/Desktop/input.txt";
string outputFile = "/Users/Steve/Desktop/cook.txt";
long positionR; // IGNORE - file position holder.

// A day is defined as x(i) and s(i) from the input.  x(i) = data available on day i.  s(i) = processing ability on day i.
struct day {
    int dataAvailable;  //x(i)
    int dataAbility;    //s(i)
};

//FUNCTION PROTOTYPES
void PrepOutputFile();   //create and clear output from previous execution
int GetNumSets();   //read in total number of serpate data-sets to process
int GetInput(vector<day>& daysList);  //read in n 'days' from file into vector of type 'days'.
void CalculateTables(vector<day> &daysList, vector<vector<int>> &Costs, vector<vector<bool>> &Choice, int numDays, int &numReboots);
void WriteOutput(vector<vector<int>> &Costs, vector<vector<bool>> &Choice, int numReboots, int numDays);


int main()
{
    PrepOutputFile();
    int numSets = GetNumSets(); //total number datasets
    
    for(int i = 0; i < numSets; i++)    //MAIN LOOP FOR EACH DATA-SET
    {
        vector<day> daysList;   //Vector to hold X(i) and S(i) from input file.
        int numDays = GetInput(daysList);   //read in input from file to daysList.  Return # elements (days) in the current dataset.
        int numReboots = 0;
        
        vector<vector<int>> Costs(numDays, vector<int>(numDays, 0));        //dynamic programming aspect which holds previously calculated values.
        vector<vector<bool>> Choice(numDays, vector<bool>(numDays, false)); //dynamic programming aspect which holds each (continue/reboot) choice.
        
        CalculateTables(daysList, Costs, Choice, numDays, numReboots);
        WriteOutput(Costs, Choice, numReboots, numDays);
    }
    return 0;
}


void PrepOutputFile()
{
    ofstream outFile;   //clear output file
    outFile.open(outputFile);
    outFile.close();
}

int GetNumSets()
{
    int numSets = 0;
    ifstream inFile;    //get num sets
    inFile.open (inputFile);
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
    inFile.open(inputFile);
    
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

void CalculateTables(vector<day> &daysList, vector<vector<int>> &Costs, vector<vector<bool>> &Choice, int numDays, int &numReboots)
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
    //Fill in table for rest of columns.
    for(int i = (numDays-2); i >= 0; i--)
    {
        for(int j = 0; j <= i; j++)
        {
            int lhs = Costs[i+1][0];
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
}

void WriteOutput(vector<vector<int>> &Costs, vector<vector<bool>> &Choice, int numReboots, int numDays)
{
    ofstream outFile;
    outFile.open(outputFile, ios::app);
    if(outFile.is_open())
    {
        outFile << Costs[0][0] << endl << numReboots << endl;
        for(int i = 0; i < numDays; i++)
        {
            if(Choice[i][0] == 0)
            {
                outFile << (i+1) << " ";
            }
        }
        outFile << "\n\n";
    }
    outFile.close();
}


