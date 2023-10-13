/**
 * Copyright 2023 Matthew Peter Smith
 *
 * This code is provided under an MIT License. 
 * See LICENSE.txt at the root of this Git repository. 
 */

#include<ctype.h>
#include<cstring>
#include<exception>
#include<fstream>
#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include<algorithm>

constexpr short EACY_NONE     = 0;
constexpr short EACY_IS_STATIC = 0b1; 
constexpr short EACY_IS_METHOD = 0b10; 
constexpr short EACY_IS_INSIDE_CLASS = 0b100;
constexpr short EACY_IS_VOID   = 0b1000;
constexpr short EACY_HAS_ARGS  = 0b10000;

struct ClassInfo
{
    std::string name; 
    size_t start; 
    size_t end; 
};

/** Finds all occurences of `to_find` in `to_search`. 
 *
 * Results, in the form of cursor positions, 
 * are pushed into the vector `res`. 
 */
void find_all(
        std::string& to_search, 
        const char* to_find,
        std::vector<size_t>& res)
{
    long cursor = -1;  
    while((cursor = to_search.find(to_find, cursor+1)) != std::string::npos)
        res.push_back(cursor);
}

/** Finds the next occurence of `to_find` in `to_search` after
 *  position `cursor`, that is on the name parenthetic level as
 *  the character at `cursor`.
 *
 *  Returns the position of the occurence within `to_find`. 
 */
size_t find_on_level(
        std::string& to_search,
        const char* to_find,
        long cursor = 0)
{
    size_t search_len = to_search.length(); 
    size_t find_len = std::strlen(to_find); 
    int level = 0; 
    while(cursor < search_len && level >= 0)
    {
        if(to_search.substr(cursor, find_len) == to_find
                && level == 0)
        {
            return cursor;
        }
        char current = to_search.at(cursor); 
        switch(current)
        {
            case '(':
            case '[':
            case '{':
                level +=  1;
                break;
            case ')':
            case ']':
            case '}':
                level += -1;
                break;
        }
        cursor += 1;
    }
    return std::string::npos;
}

/** Finds all occurences of `to_find` in `to_search` 
 *  on the same parenthetic level at the character at
 *  `cursor+1`.
 *
 *  Positions of each occurence are pushed to vector `res`.
 */
void find_all_on_level(
        std::string& to_search,
        const char* to_find,
        std::vector<size_t>& res,
        long cursor = -1)
{
    while((cursor = find_on_level(to_search, to_find, cursor+1)) != std::string::npos)
        res.push_back(cursor);
}

/** Get a substring up to the next whitespace on the same
 *  parenthetic level as the character at `offset`. 
 */
std::string get_lump(
        std::string& source, 
        long offset)
{
    // find the next whitespace on the same level
    size_t end;
    {
        size_t space = find_on_level(source, " ", offset + 1);
        size_t tab   = find_on_level(source, "\t", offset + 1);
        size_t nl    = find_on_level(source, "\n", offset + 1);
        size_t cr    = find_on_level(source, "\r", offset + 1);
        size_t op    = find_on_level(source, "(", offset + 1); 
        end = space < tab ? space : tab;
        end = nl    < end ? nl    : end;
        end = cr    < end ? cr    : end;
        end = op    < end ? op    : end;
    }
    std::string res;
    return res.assign(source.substr(offset, end - offset));  
}

/** Parse the source file and print out the same file
 *  with the inserted macros. 
 */
void process_source(const char* source_filename) 
{
    // read source file into memory
    std::string source;
    {
        std::ifstream reader(source_filename); 
        if(!reader.good())
        {
            std::string err = std::string("Failed to open `")
                + source_filename
                + ".";
            throw std::runtime_error(err);
        }
        std::stringstream contents; 
        contents << reader.rdbuf();
        source.assign(contents.str()); 
        reader.close();
    }

    // find the EAC functions
    std::vector<size_t> eac_starts;
    find_all(source, "EAC ",  eac_starts); 

    std::vector<std::string*>   eac_funcs;
    std::vector<std::string*>   eac_exports; 
    std::vector<std::string*>   eac_export_decs; 
    std::vector<size_t>         eac_body_starts; 
    std::vector<size_t>         eac_body_ends; 
    std::vector<short>          eac_props;
    std::vector<long>           eac_classes;

    // get position information on EAC classes
    std::vector<size_t> class_starts; 
    std::vector<ClassInfo> class_info; 
    find_all(source, "EAC_CLASS", class_starts); 

    for(size_t eac_class : class_starts)
    {
#define SKIP_WHITESPACE(text, k)\
        while(std::isblank(text.at(k))) k += 1;
            
#define FIND_WHITESPACE(text, k)\
        while(!std::isblank(text.at(k))) k += 1;

        size_t cursor = eac_class;
        FIND_WHITESPACE(source, cursor);
        SKIP_WHITESPACE(source, cursor);
        FIND_WHITESPACE(source, cursor);
        SKIP_WHITESPACE(source, cursor);

        size_t name_end = cursor;
        while(std::isalnum(source.at(name_end)) || source.at(name_end) == '_')
            name_end += 1;

        std::string name = source.substr(cursor, name_end - cursor); 

        size_t open  = find_on_level(source, "{", eac_class + 1);
        size_t close = find_on_level(source, "}", open + 1) + 1; 
        ClassInfo info;
        info.name.assign(name);
        info.start = eac_class; 
        info.end   = close;
        class_info.push_back(info);
    }

    for(size_t eac : eac_starts)
    {
        // figure out the binary properties of the function 
        short eac_prop = EACY_NONE; 
        size_t cursor = eac + 3; 
        SKIP_WHITESPACE(source, cursor); 
        std::string type_str = get_lump(source, cursor); 
        if(type_str == "static")
        {
            eac_prop |= EACY_IS_STATIC;
            cursor += 6;
            SKIP_WHITESPACE(source, cursor); 
            type_str = get_lump(source, cursor); 
        }
        if(type_str == "void")
        {
            eac_prop |= EACY_IS_VOID;
        }

        cursor += type_str.length();
        SKIP_WHITESPACE(source, cursor); 
        std::string name_str = get_lump(source, cursor); 

        std::string class_str; 
        size_t colon = name_str.find("::", 0);
        if(colon != std::string::npos)
        {
            eac_prop |= EACY_IS_METHOD; 
            class_str.assign(name_str.substr(0, colon)); 
            cursor += class_str.length() + 2; 
            name_str = get_lump(source, cursor); 
        }
        long class_index = -1;

        // start calculating source properties
        // e.g. function names, class names, args, etc.
        for(ClassInfo& info : class_info)
        {
            if(eac < info.start || eac > info.end) continue; 
            eac_prop |= EACY_IS_METHOD;
            eac_prop |= EACY_IS_INSIDE_CLASS;;
            class_str.assign(info.name);
            class_index = &info - (ClassInfo*)class_info.data();
        }
        eac_classes.push_back(class_index);

        cursor += name_str.length(); 
        while(source.at(cursor) != '(')
            cursor += 1; 
        size_t args_open    = cursor;
        size_t args_close   = find_on_level(source, ")", args_open + 1);
        std::string args_str = source.substr(args_open + 1, args_close - args_open - 1);

        std::vector<size_t> commas; 
        find_all_on_level(source, ",", commas, args_open + 1); 
        if(!commas.empty())
            commas.push_back(args_close); 

        std::stringstream arg_names_stream; 
        for(size_t comma : commas)
        {
            size_t arg_name_end = comma - 1;
            while(std::isblank(source.at(arg_name_end)))
                arg_name_end += -1;
            arg_name_end += 1;

            size_t arg_name_start = arg_name_end - 1;
            while(!std::isblank(source.at(arg_name_start)))
                arg_name_start += -1;
            arg_name_start += 1;

            std::string arg_name = source.substr(arg_name_start, 
                    arg_name_end - arg_name_start);

            if(!arg_names_stream.str().empty())
                arg_names_stream << ", ";
            arg_names_stream << arg_name;
        }
        std::string arg_names_str = arg_names_stream.str();

        if(!arg_names_str.empty())
            eac_prop |= EACY_HAS_ARGS;

        size_t body_start = find_on_level(source, "{", args_close + 1);
        size_t body_end   = find_on_level(source, "}", body_start + 1);

        eac_body_starts.push_back(body_start);
        eac_body_ends.push_back(body_end);

        // build function macro invocation
        std::stringstream eac_func_stream;
        if(eac_prop & EACY_IS_METHOD) 
        {
            if(eac_prop & EACY_IS_VOID)
                eac_func_stream << "EAC_VOID_METHOD(";
            else
                eac_func_stream << "EAC_METHOD(";
            eac_func_stream << class_str << ", ";
        }
        else if((eac_prop & EACY_IS_VOID) && (eac_prop &EACY_IS_STATIC))
            eac_func_stream << "EAC_STATIC_VOID_FUNC(";
        else if(eac_prop & EACY_IS_VOID)
            eac_func_stream << "EAC_VOID_FUNC(";
        else if(eac_prop & EACY_IS_STATIC)
            eac_func_stream << "EAC_STATIC_FUNC("; 
        else
            eac_func_stream << "EAC_FUNC(";

        eac_func_stream << name_str << ", ";

        if(!(eac_prop & EACY_IS_VOID))
            eac_func_stream << "EAC_WRAP(" << type_str << "), ";

        if((eac_prop & EACY_HAS_ARGS) && (eac_prop & EACY_IS_METHOD))
            eac_func_stream << "EAC_WRAP(, " << arg_names_str << "))";
        else if(eac_prop & EACY_HAS_ARGS)
            eac_func_stream << "EAC_WRAP(" << arg_names_str << "))";
        else
            eac_func_stream << "EAC_WRAP(/**/))";


        // build export macro invocation
        std::stringstream eac_export_stream; 
        if(eac_prop & EACY_IS_METHOD)
        {
            if(eac_prop & EACY_IS_VOID)
                eac_export_stream << "EAC_VOID_METHOD_EXPORT(";
            else
                eac_export_stream << "EAC_METHOD_EXPORT(";
            eac_export_stream << class_str << ", ";
        }
        else
        {
            if(eac_prop & EACY_IS_VOID)
            {
                eac_export_stream << "EAC_VOID_EXPORT(";
            }
            else
            {
                eac_export_stream << "EAC_EXPORT(";
            }
        }

        eac_export_stream << name_str << ", ";

        if(!(eac_prop & EACY_IS_VOID))
        {
            eac_export_stream << "EAC_WRAP(" << type_str << "), ";
        }

        if((eac_prop & EACY_HAS_ARGS) && (eac_prop & EACY_IS_METHOD))
        {
            eac_export_stream << "EAC_WRAP(, " << args_str << "), "
                << "EAC_WRAP(" << arg_names_str << "))";
        }
        else if(eac_prop & EACY_HAS_ARGS)
            eac_export_stream << "EAC_WRAP(" << args_str << "), "
                << "EAC_WRAP(" << arg_names_str << "))";
        else
            eac_export_stream << "EAC_WRAP(/**/), EAC_WRAP(/**/))";


        std::string* exp = new std::string; 
        exp->assign(eac_export_stream.str());
        eac_exports.push_back(exp);

        // methods require an extra declaration of the
        // export wrapper
        if(eac_prop & EACY_IS_METHOD)
        {
            std::stringstream decl_stream;
            if(eac_prop & EACY_IS_VOID)
                decl_stream << "EAC_VOID_METHOD_EXPORT_DEC(";
            else
                decl_stream << "EAC_METHOD_EXPORT_DEC(";
            decl_stream << class_str << ", " << name_str << ", "; 
            if(!(eac_prop & EACY_IS_VOID))
                decl_stream << "EAC_WRAP(" << type_str << "), ";

            if(eac_prop & EACY_HAS_ARGS)
                decl_stream << "EAC_WRAP(, " << args_str << "))";
            else
                decl_stream << "EAC_WRAP(/**/))";

            std::string* decl = new std::string; 
            decl->assign(decl_stream.str());
            eac_export_decs.push_back(decl);
        }

        std::string* marker = new std::string; 
        marker->assign(eac_func_stream.str()); 

        eac_funcs.push_back(marker);
        eac_props.push_back(eac_prop);
    }

    // construct the processed version of the source
    std::string output; 
    output.assign(source); 

    size_t offset = 0; 
    for(int k = 0; k < eac_starts.size(); k++)
    {
        size_t eac      = eac_starts[k]; 
        size_t fstart   = eac + 3;
        while(std::isblank(source.at(fstart)))
            fstart += 1; 

        if(eac_props[k] & EACY_IS_INSIDE_CLASS)
        {
            long class_index = eac_classes[k]; 
            ClassInfo& info  = class_info[class_index];
            output = output.substr(0, info.start + offset)
                + *eac_export_decs[k] + "\n"
                + output.substr(info.start + offset);
            offset += eac_export_decs[k]->length() + 1;
        }
        else if(eac_props[k] & EACY_IS_METHOD)
        {
            output = output.substr(0, eac + offset)
                + *eac_export_decs[k] + "\n"
                + output.substr(eac + offset);
            offset += eac_export_decs[k]->length() + 1;
        }

        size_t bs   = eac_body_starts[k];
        output = output.substr(0, eac + offset) 
            + output.substr(fstart + offset, bs - fstart + 1) 
            + *eac_funcs[k] + output.substr(bs + offset + 1);
        offset += eac_funcs[k]->length() - (fstart - eac);

        if(eac_props[k] & EACY_IS_INSIDE_CLASS)
        {
            long class_index = eac_classes[k]; 
            ClassInfo& info  = class_info[class_index];
            output = output.substr(0, info.end + offset + 1)
                + *eac_exports[k]
                + output.substr(info.end + offset + 1);
        }
        else
        {
            size_t be   = eac_body_ends[k];
            output = output.substr(0, be + offset + 1)
                + *eac_exports[k]
                + output.substr(be + offset + 1);
            offset += eac_exports[k]->length(); 
        }
    }

    // print the result
    std::cout << output << std::endl;
}

int main(int argc, const char** argv)
{
    if(argc < 2)
        std::cerr << "Please provide the name of a file." << std::endl;
    process_source(argv[1]);
    return 0;
}
