﻿#include<hgl/io/TextOutputStream.h>

namespace hgl
{
    namespace logger
    {
        io::TextOutputStream *CreateTextOutputStream(io::OutputStream *os)
        {
            return(new io::UTF16LETextOutputStream(os));
        }
    }//namespace logger
}//namespace hgl
