#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
#include <windows.h>
#include <shellapi.h>
#include <minwinbase.h>
#include <iomanip>
#include <ios>
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "User32.lib")

using namespace std;

struct file_size_structure
{
    long double file_size;
    string file_unit;
} storage_output_file_size;

file_size_structure ConvertFileSizeUnit(__int64 file_size);

int main(int argc, char const *argv[])
{
    ShowWindow(FindWindow("ConsoleWindowClass", argv[0]), 0);

    string file_name = "log.txt";
    fstream file;
    file.open(file_name, ios::in);
    bool file_exist_state = file.is_open();
    if (file_exist_state == false)
    {
        file.clear();
        file.close();
        file.open(file_name, ios::app); // app模式无法自定义光标位置，所以只用来创建文件。
    }

    file.clear();
    file.close();
    file.open(file_name, ios::in | ios::out | ios::ate);

    time_t time_point_since_epoch;
    tm *time_now;
    char first_line[80], second_line[80];
    short execution_time = 0, read_execution_time = 0;

    SHQUERYRBINFO recycle_bin_information;
    ZeroMemory(&recycle_bin_information, sizeof(recycle_bin_information));
    recycle_bin_information.cbSize = sizeof(recycle_bin_information);

    string recycle_bin_query_state, recycle_bin_operation, operation_execution_state;
    string bin_file_unit;
    __int64 bin_file_size = 0, bin_file_number = 0;
    long double bin_file_convert_size = 0;

    fstream::pos_type cursor_position = 0;

    if (file_exist_state == false)
    {
        time_point_since_epoch = time(nullptr);
        time_now = localtime(&time_point_since_epoch);
        file << "本程序初次运行于：" << time_now->tm_year + 1900 << "年"
             << time_now->tm_mon + 1 << "月" << time_now->tm_mday << "日"
             << time_now->tm_hour << "时" << time_now->tm_min << "分"
             << time_now->tm_sec << "秒" << endl;

        if (SHQueryRecycleBin(NULL, &recycle_bin_information) == S_OK)
        {
            recycle_bin_query_state = "查询成功。";
            bin_file_size = recycle_bin_information.i64Size;
            bin_file_number = recycle_bin_information.i64NumItems;

            if (bin_file_number > 0)
            {
                storage_output_file_size = ConvertFileSizeUnit(bin_file_size);
                bin_file_convert_size = storage_output_file_size.file_size;
                bin_file_unit = storage_output_file_size.file_unit;
                recycle_bin_operation = "清空回收站；";
                if (SHEmptyRecycleBin(NULL, NULL, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND) == S_OK)
                {
                    operation_execution_state = "删除成功。";
                    execution_time = 1;
                }
                else
                {
                    operation_execution_state = "删除失败。";
                    execution_time = 0;
                }
            }
            else
            {
                recycle_bin_operation = "无；";
                operation_execution_state = "无。";
                bin_file_convert_size = 0;
                bin_file_unit = "；";
                bin_file_number = 0;
                execution_time = 0;
            }
        }
        else
        {
            recycle_bin_query_state = "查询失败。";
            recycle_bin_operation = "无；";
            operation_execution_state = "无。";
            bin_file_convert_size = 0;
            bin_file_unit = "；";
            bin_file_number = 0;
            execution_time = 0;
        }

        cursor_position = file.tellp();
        file << "程序成功执行「清空回收站」操作的次数：" << execution_time << endl
             << endl;
        file << "具体操作记录：" << endl;
        file << "检测时间：" << time_now->tm_year + 1900 << "年"
             << time_now->tm_mon + 1 << "月" << time_now->tm_mday << "日"
             << time_now->tm_hour << "时" << time_now->tm_min << "分"
             << time_now->tm_sec << "秒。";
        file << "回收站查询状态：" << recycle_bin_query_state;
        file << "执行的操作：" << recycle_bin_operation;
        file << "状态：" << operation_execution_state;
        if (bin_file_convert_size == 0)
            file << "删除的文件大小：" << bin_file_convert_size << bin_file_unit;
        else if ((bin_file_convert_size - int(bin_file_convert_size) == 0))
            file << "删除的文件大小：" << bin_file_convert_size << bin_file_unit;
        else
            file << "删除的文件大小：" << fixed << setprecision(2) << bin_file_convert_size << bin_file_unit;
        file << "数量：" << bin_file_number << "。" << endl;
        file_exist_state = true;
    }
    else
    {
        file.seekg(0, ios::beg);
        file.getline(first_line, 80);
        file.getline(second_line, 80);

        file.seekp(0, ios::beg);
        file << first_line << endl;
        cursor_position = file.tellp();
        file.seekg(0, ios::end);

        for (short i = 0; second_line[i] != '\0'; i++)
            if (second_line[i] >= '0' && second_line[i] <= '9')
            {
                read_execution_time = read_execution_time * 10 + second_line[i] - '0'; // 字符 0 的十进制为 48
            }
        execution_time = read_execution_time;
    }

    while (1)
    {
        this_thread::sleep_for(chrono::seconds(1));

        time_point_since_epoch = time(nullptr);        // 获取自纪元起的时间点（秒数）
        time_now = localtime(&time_point_since_epoch); // 将时间点转换成本地日历时间，并存储在 tm 结构体

        if (time_now->tm_hour == 20 && time_now->tm_min == 0 && time_now->tm_sec == 0)
        {
            if (SHQueryRecycleBin(NULL, &recycle_bin_information) == S_OK)
            {
                recycle_bin_query_state = "查询成功。";
                bin_file_size = recycle_bin_information.i64Size;
                bin_file_number = recycle_bin_information.i64NumItems;

                if (bin_file_number > 0) // 删除的文件大小可能是 0，但数量一定大于 0。
                {
                    storage_output_file_size = ConvertFileSizeUnit(bin_file_size);
                    bin_file_convert_size = storage_output_file_size.file_size;
                    bin_file_unit = storage_output_file_size.file_unit;
                    recycle_bin_operation = "清空回收站；";
                    if (SHEmptyRecycleBin(NULL, NULL, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND) == S_OK)
                    {
                        operation_execution_state = "删除成功。";
                        execution_time++;
                        file.seekp(cursor_position);
                        file << "程序成功执行「清空回收站」操作的次数：" << execution_time << endl;
                        file.seekp(0, ios::end);
                    }
                    else
                    {
                        operation_execution_state = "删除失败。";
                    }
                }
                else
                {
                    recycle_bin_operation = "无；";
                    operation_execution_state = "无。";
                    bin_file_convert_size = 0;
                    bin_file_unit = "；";
                    bin_file_number = 0;
                }
            }
            else
            {
                recycle_bin_query_state = "查询失败。";
                recycle_bin_operation = "无；";
                operation_execution_state = "无。";
                bin_file_convert_size = 0;
                bin_file_unit = "；";
                bin_file_number = 0;
            }

            file << "检测时间：" << time_now->tm_year + 1900 << "年"
                 << time_now->tm_mon + 1 << "月" << time_now->tm_mday << "日"
                 << time_now->tm_hour << "时" << time_now->tm_min << "分"
                 << time_now->tm_sec << "秒。";
            file << "回收站查询状态：" << recycle_bin_query_state;
            file << "执行的操作：" << recycle_bin_operation;
            file << "状态：" << operation_execution_state;
            if (bin_file_convert_size == 0)
                file << "删除的文件大小：" << resetiosflags(ios_base::fixed) << bin_file_convert_size << bin_file_unit;
            else if ((bin_file_convert_size - int(bin_file_convert_size) == 0))
                file << "删除的文件大小：" << resetiosflags(ios_base::fixed) << bin_file_convert_size << bin_file_unit;
            else
                file << "删除的文件大小：" << fixed << setprecision(2) << bin_file_convert_size << bin_file_unit;
            file << "数量：" << bin_file_number << "。" << endl;
        }
    }
    return 0;
}

file_size_structure ConvertFileSizeUnit(__int64 file_size)
{
    string output_file_unit;
    long double output_file_size = 0;
    const long double constant = 1024;

    if (file_size < constant)
    {
        output_file_size = file_size;
        output_file_unit = " Byte；";
    }
    else if (file_size >= constant && file_size < constant * constant)
    {
        output_file_size = file_size / constant;
        output_file_unit = " KB；";
    }
    else if (file_size >= constant * constant && file_size < constant * constant * constant)
    {
        output_file_size = file_size / (constant * constant);
        output_file_unit = " MB；";
    }
    else if (file_size >= constant * constant * constant && file_size < constant * constant * constant * constant)
    {
        output_file_size = file_size / (constant * constant * constant);
        output_file_unit = " GB；";
    }
    else if (file_size >= constant * constant * constant * constant && file_size < constant * constant * constant * constant * constant)
    {
        output_file_size = file_size / (constant * constant * constant * constant);
        output_file_unit = " TB；";
    }
    return {output_file_size, output_file_unit};
}
