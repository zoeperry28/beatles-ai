#include <string>
#include <map>
#include <boost/dynamic_bitset.hpp>

#ifndef __HELPER_H__
#define __HELPER_H__

bool IsWavFile(std::string path); 
bool IsFolder(std::string path) ;

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
        //std::map<std::string, std::string> FixTypes(std::map<std::string, void *> new_line);
        std::map<std::string, std::vector<std::string>> store; 
        bool VerifyHeaders(std::map<std::string, std::string>);  
        std::string CastVoid(void * to_cast);
};

#endif // __HELPER_H__
