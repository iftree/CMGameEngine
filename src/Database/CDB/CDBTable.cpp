﻿#include<hgl/db/CDBTable.h>
#include<hgl/io/DataInputStream.h>
#include<hgl/io/FileInputStream.h>
#include<hgl/LogInfo.h>

namespace hgl
{
    namespace db
    {
#ifdef LOG_CDB_LOADER_LOG
        void CDBFiledOutputLog(int,const CDBField *f);
#endif//LOG_CDB_LOADER_LOG

        namespace
        {
            constexpr uchar cdb_flag[]  ="CDB Table\x1A";
            constexpr uint  cdb_flag_len=sizeof(cdb_flag)-1;
            constexpr float cdb_ver     =6.0;
        }

        void CDBTable::InitPrivate()
        {
            data_count=0;
        }

        CDBTable::CDBTable()
        {
            InitPrivate();
        }

        /**
        * 从流中加载一个表
        */
        bool CDBTable::Load(io::DataInputStream *dis)
        {
            uchar flag[10];
            float ver;
            UTF16StringList field_name;
            UTF16StringList field_list;
            FieldTypeList field_type;

            if(dis->ReadFully(flag,cdb_flag_len)!=cdb_flag_len
             ||memcmp(flag,cdb_flag,cdb_flag_len))
            {
                LOG_ERROR(OS_TEXT("CDB表格文件头不正确"));
                return(false);
            }

            if(!dis->ReadFloat(ver)
             ||ver!=cdb_ver)
            {
                LOG_ERROR(OS_TEXT("CDB表格文件版本不被支持!"));
                return(false);
            }

            LoadUTF16LEStringList(field_name,dis);        //字段名称
            LoadUTF16LEStringList(field_list,dis);        //字段类型字符串

            ParseFieldType(field_type,field_list);        //将字段类型由字符串转换为程序用二进制数据

            dis->ReadInt32(data_count);                   //数据数量

            FieldType *ft=field_type.GetData();

            for(int i=0;i<field_list.GetCount();i++)
            {
                CDBField *f=CreateCDBField(field_name[i],*ft,dis,data_count);

                ++ft;

                if(f)
                    Fields.Add(field_name[i],f);
                else
                    LOG_ERROR(U16_TEXT("Create Field failed,name \"") + field_name[i]);
            }
#ifdef LOG_CDB_LOADER_LOG
            {
                auto &wfl=Fields.GetList();

                auto **pwf=wfl.GetData();

                for(int i=0;i<wfl.GetCount();i++)
                {
                    CDBFiledOutputLog(i,(*pwf)->second);    //输出日志
                    pwf++;
                }
            }
#endif//LOG_CDB_LOADER_LOG
            return(true);
        }

        bool CDBTable::Load(const OSString &filename)
        {
            if(filename.IsEmpty())
                return(false);

            io::FileInputStream fis;

            if(!fis.Open(filename))
                return(false);

            io::LEDataInputStream dis(&fis);

            return Load(&dis);
        }
    }//namespace db
}//namespace hgl
