#include <string>
#include <map>
#include <boost/dynamic_bitset.hpp>
#include <boost/cstdfloat.hpp>
#include <boost/any.hpp>
#include <vector>
#ifndef __HELPER_H__
#define __HELPER_H__

bool IsWavFile(std::string path); 
bool IsCSVFile(std::string path);
bool IsFolder(std::string path) ;
float StdDev(const std::vector<float>& n);
std::vector<std::string> SplitByDelimiter(std::string to_check, std::string delim);
std::vector<float> LinSpace(float start, float stop, int num = 50);
std::vector<float> Zeros(int ind);
std::vector<float> Arrange(int start, int stop, int step);
std::vector<float> AddAtIndex(std::vector<float> data, std::vector<float> to_add, int start, int end);
class LoadingBar
{
public:

    float Percent = 0;
    long double Incr = 0;
    long double Items = 0;
    int Counted = 0;
    int Scale = 1;
    std::string Title = "";
    std::string subtext = "";
    LoadingBar(std::string title, int NoOfItems, int scale=1)
    {
        Items = NoOfItems;
        Scale = scale;
        CalculateIncr();
        WriteTitle(title);
    }
    ~LoadingBar()
    {
        printf("\n");
    }

    void WriteHeader()
    {
        printf("%s\n", Title.c_str());
        WriteBar();
    }
    void WriteBar(int MaxSize = -1)
    {
        if (MaxSize == -1)
        {
            MaxSize = Items;
        }
        std::string out;

        int to_populate = (int)Percent / Scale;
        for (int i = 0; i < 100 / Scale; i++)
        {
            if (i < to_populate)
            {
                out = out + "|";
            }
            else
            {
                out = out + " ";
            }
        }
        printf("\r[%s] %d %s", out.c_str(), (int)Percent, "% ");
    }

    void LogProgress1()
    {
        Percent = Percent + Incr;
        Counted++;
        WriteBar();
    }

    void LogProgressMultiple(int Amount)
    {
        Percent = Percent + (Incr * Amount);
        Counted += Amount;
    }

    void WriteTitle(std::string s)
    {
        Title = s;
        WriteHeader();
    }

private:
    void CalculateIncr()
    {
        Incr = 100 / Items;
    }
};


enum Type
{
    E_NONE   = 0,
    E_STRING = 1,
    E_NUM = 2
};
struct CSVHeader 
{
    std::string text;
    Type underlying_type = E_NONE;
};

class CSVWriter
{
    public: 
        CSVWriter(std::vector<std::string> headers)
        {
            SetHeader(headers);
        }

        ~CSVWriter()
        {

        }
        void SetHeader(std::vector<std::string> headers);

        bool AddLine(const std::map<std::string, std::string>& new_line);
        bool AddLine(const std::map<std::string, float>& new_line);

        void Export(std::string filename);
    private:
        std::vector<CSVHeader> head;
        //std::map<std::string, std::string> FixTypes(std::map<std::string, void *> new_line);
        std::map<std::string, std::vector<std::string>> store; 
        bool VerifyHeaders(std::map<std::string, std::string> data);

        std::vector<CSVHeader> GetAllHeaders() ;

        void SetHeaderType(CSVHeader new_header);

        Type DetermineType(std::pair<std::string, float>);
        Type DetermineType(std::pair<std::string, std::string> to_check);
};

#endif // __HELPER_H__
