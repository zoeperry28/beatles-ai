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
#include <boost/any.hpp>




float StdDev(const std::vector<float>& n) 
{
    std::vector<float> final;

    float E = 0;
    for (int i = 0; i < n.size(); i++) {
        E += n[i];
    }
    float X = E / n.size();

    float v = 0;
    for (int i = 0; i < n.size(); i++) {
        v += std::pow((n[i] - X), 2);
    }
    v = v / (n.size() - 1);

    return std::sqrt(v);
}

bool IsWavFile(std::string path) 
{
    return std::filesystem::path(path).extension() == ".wav";
}

bool IsFolder(std::string path) 
{
    return std::filesystem::is_directory(path);
}

void CSVWriter::SetHeaderType(CSVHeader new_header)
{
    for (int i = 0 ; i < head.size() ; i++)
    {
        if (head[i].text == new_header.text)
        {
            head[i].underlying_type = new_header.underlying_type;
        }
    }
}

Type GetType(std::string val)
{
    bool check = true; 

    for (const auto& c : val)
    {
        if (std::isdigit(c) == false && c != '.')
        {
            check = false;
        }
    }

    if (check == true)
    {
        return E_NUM;
    }
    else
    {
        return E_STRING;
    }
}

// need to check not only if this header exists, but also whether the data itself is the correct type. 
// if the type has not been determined yet, this will need to be decided. 
bool CSVWriter::VerifyHeaders(std::map<std::string, std::string> data)
{
    const std::vector<CSVHeader> HEADERS = head;

    bool IsValidHeader = true;
    for (int i = 0 ; i < (int)HEADERS.size(); i++)
    {
        auto v = GetType(data.at(HEADERS[i].text));

        if (data.count(HEADERS[i].text) > 0 && HEADERS[i].underlying_type == E_NONE)
        {
            bool val = GetType(HEADERS[i].text);
            if (val == true)
            {
                SetHeaderType({
                    HEADERS[i].text, 
                    E_NUM
                });  
            }
            else
            {
                SetHeaderType({
                    HEADERS[i].text, 
                    E_STRING
                });
            }
        }
        else if (v != HEADERS[i].underlying_type)
        {
            IsValidHeader = false;
            std::cout<< "[CSV ] [DATA = " << data.at(HEADERS[i].text)  
                     << "] REJECTED - Please ensure data types are consistent!\n"; 
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
    
    std::vector<CSVHeader> ne;
    for (int i = 0 ; i < (int)headers.size(); i++)
    {
        CSVHeader h;
        h.text = headers[i];
        ne.push_back(h);
        std::vector<std::string> empty = {}; 
        store.insert({h.text, empty});
    }
    head = ne;
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
    bool out = false;
    std::map<std::string, std::string> to_return;
    for(auto const& imap: new_line)
    {
        to_return.insert({imap.first, std::to_string(imap.second)});
    }
    out = AddLine(to_return);
    return out;
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
