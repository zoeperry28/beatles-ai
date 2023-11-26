#include <filesystem>
#include <vector>
#include "helper.hpp"
#include <boost/dynamic_bitset.hpp>
#include <utility>
#include <iostream>
#include <typeinfo>
#include <fstream>
#include <cassert>
#include <iostream>

bool IsWavFile(std::string path) 
{
    return std::filesystem::path(path).extension() == ".wav";
}

bool IsFolder(std::string path) 
{
    return std::filesystem::path(path).has_extension();
}

std::string CSVWriter::CastVoid(void * to_cast)
{
    char * x = reinterpret_cast<char *>(&to_cast);
    return std::string(x);
}

bool IsFloat(float to_check)
{
    bool to_return = false;

    int to_mod = (int) to_check;
    if (to_check / to_mod != 1 && to_check != 0)
    {
        to_return = true; 
    }
    else if (to_check == 0 && to_mod == 0)
    {
        to_return = false;
    }

    return to_return;
}

//std::map<std::string, std::string> CSVWriter::FixTypes(std::map<std::string, void *> new_line)
//{
//    std::map<std::string, std::string> new_values;
//    for(std::map<std::string, float>::iterator it = new_line.begin(); it != new_line.end(); ++it) 
//    {
//        new_values.insert({it->first, std::to_string(it->second)});
//    };
//    return new_values;
//}

bool CSVWriter::VerifyHeaders(std::map<std::string, std::string> data)
{
    bool IsValidHeader = true;
    for(std::map<std::string, std::string>::iterator it = data.begin(); it != data.end(); ++it) 
    {
        if (store.find(it->first) == store.end())
        {
            IsValidHeader = false;
        } 
    }
    return IsValidHeader;
}

void CSVWriter::SetHeader(std::vector<std::string> headers)
{   
    if (store.empty() == false)
    {
        store.clear();
    }
    for (int i = 0 ; i < (int)headers.size(); i++)
    {
        std::vector<std::string> ne;
        std::pair<std::string, std::vector<std::string>> v = {headers[i], ne}; 
        store.insert(v);
    }
}

bool CSVWriter::AddLine(const std::map<std::string, std::string>& new_line)
{
    if (!store.empty() && VerifyHeaders(new_line))
    {
        for(const auto& entry : new_line) 
        {
            auto it = store.find(entry.first);
            if (it != store.end())
            {
                it->second.push_back(entry.second);
            }
            else
            {
                // Handle the case where the key is not found in store
                // You may choose to ignore, add a default value, or handle it based on your requirements.
            }
        }
        return true;
    }
    return false;
}


bool CSVWriter::AddLine(const std::map<std::string, float>& new_line)
{
    std::map<std::string, std::string> to_return;
    for(auto const& imap: new_line)
    {
        if (IsFloat(imap.second))
        {
            to_return.insert({imap.first, std::to_string(imap.second)});
        }
        else
        {
            to_return.insert({imap.first, std::to_string((int) imap.second)});
        }
    }
    return AddLine(to_return);
}

void CSVWriter::Export(std::string filename)
{
    std::string to_write = ""; 
    const int no_of_headers = store.size();
    int no_of_entries = 0;
    int heading_count = 0;

    std::vector<std::vector<std::string>> data_store; 
    for(std::map<std::string, std::vector<std::string>>::iterator it = store.begin(); it != store.end(); ++it) 
    {
        if (heading_count == 0)
        {
            no_of_entries = it->second.size();
        }
        if (heading_count != no_of_headers - 1)
        {
            to_write = to_write + it->first + ",";
        }
        else
        {
            to_write = to_write + it->first + "\n";
        }
        data_store.push_back(it->second);
        heading_count++;
    }

    for (int i = 0 ; i < no_of_entries; i++)
    {
        for (int j = 0 ; j < no_of_headers; j++)
        {
            if (j != no_of_headers-1)
            {
                to_write = to_write + data_store.at(j).at(i) + ",";
            }
            else
            {
                to_write = to_write + data_store.at(j).at(i) + "\n";
            }
        }
    }
    std::cout << to_write;
    std::ofstream myfile;
    myfile.open (filename);
    myfile << to_write;
    myfile.close();
}
