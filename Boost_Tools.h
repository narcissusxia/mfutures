#ifndef __BOOST_TOOLS_H__  
#define __BOOST_TOOLS_H__  
#include <boost/locale.hpp>  
  
namespace boosttoolsnamespace  
{  
    class CBoostTools  
    {  
    public:  
          
        //��װ string between( string const &text,string const &to_encoding,  
        //string const &from_encoding,method_type how = default_method);   
        //ֱ�Ӵ���gbkתutf8����  
        static std::string gbktoutf8(std::string const &text);  
        //ֱ�Ӵ���utf8תgbk  
        static std::string utf8togbk(std::string const &text);  
        //ֱ�Ӵ���big5תutf8  
        static std::string big5toutf8(std::string const &text);  
        //ֱ�Ӵ���utf8תbig5  
        static std::string utf8tobig5(std::string const &text);    
    };  
  
}  
#endif  
