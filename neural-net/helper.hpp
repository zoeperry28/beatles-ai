#include <string>
#include <map>
#include <boost/dynamic_bitset.hpp>
#include <boost/any.hpp>
#include <vector>
#ifndef __HELPER_H__
#define __HELPER_H__

bool IsWavFile(std::string path); 
bool IsFolder(std::string path) ;
float StdDev(const std::vector<float>& n);

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
