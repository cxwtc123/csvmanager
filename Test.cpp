/*! 
*  \file	  Test.cpp
*  \brief     CSV文件管理类 测试代码
*  \details   CSV文件管理类 测试代码，对CSVManage类的方法进行测试
*  \author    cxw
*  \version   1.0
*  \date      2012-7-10
*  \bug       报告BUG，请联系 cxwtc123@gmail.com
*  \copyright free
*/

#include <iostream>
#include <fstream>
#include "CSVManage.h"

using namespace std;

void newFileTest(CXW::CSVManage& csvMan);
void loadFileTest(CXW::CSVManage& csvMan);
void getRowFromStreamTest(void);
void addRowToStreamTest(void);
void sortTest(CXW::CSVManage& csvMan);


int main(void)
{
	CXW::CSVManage csvMan;

// 	newFileTest(csvMan);

	loadFileTest(csvMan);

	//getRowFromStreamTest();
	//addRowToStreamTest ();

	//sortTest(csvMan);

	return 0;
};



void newFileTest(CXW::CSVManage& csvMan)
{
	CXW::CSVManage::Title title;
	title.push_back ("编号");
	title.push_back ("测点名称");
	title.push_back ("时间戳");
	title.push_back ("数值");
	title.push_back ("质量码");

	csvMan.newFile ("csvCreate.csv",title);

	CXW::CSVManage::Row row;
	char buf[32];

	for (int i = 0 ; i < 1000000 ; i++)
	{
		row.clear ();

		row.push_back (_itoa (i,buf,10));		
		row.push_back ("TagName001");
		row.push_back ("2012-7-12 9:23:00");
		row.push_back (_itoa (rand(),buf,10));
		row.push_back (_itoa (i,buf,16));
		csvMan.addRow (row);
	}

	csvMan.save();
}

void loadFileTest(CXW::CSVManage& csvMan)
{
	// 	首次测试验证
	csvMan.loadFile ("test.csv",true);

	for (int i = 0 ;i < csvMan.getRowSize () ; i++)
	{
		for (int j = 0 ;j < csvMan.getColSize () ; j++)
		{
			string str = csvMan.getXY (i,j).c_str() ;
// 			str += "add";
// 			csvMan.modify (i,j,str.c_str());
		}
	}

	cout << "csvMan.loadFile (\"test.csv\",true);   test" << endl;
	cout << "hastitle ? " << csvMan.hasTitle () << endl;
	cout << "datasize:   row = " << csvMan.getRowSize () << 
		"  col = " << csvMan.getColSize () << endl;

	csvMan.saveToFile ("testsave.csv");

	cout << "csvMan.saveToFile (\"testsave.csv\");   test" << endl;
	cout << "--------------------------------------------" << endl << endl;

	// 	二次测试验证

	//csvMan.loadFile ("test.csv");

	//for (int i = 0 ;i < csvMan.getRowSize () ; i++)
	//{
	//	for (int j = 0 ;j < csvMan.getColSize () ; j++)
	//	{
	//		string str = csvMan.getXY (i,j).c_str() ;
	//		str += "add";
	//		csvMan.modify (i,j,str.c_str());
	//	}
	//}

	//cout << "csvMan.loadFile (\"test.csv\");   test" << endl;
	//cout << "hastitle ? " << csvMan.hasTitle () << endl;
	//cout << "datasize:   row = " << csvMan.getRowSize () << 
	//	"  col = " << csvMan.getColSize () << endl;

	//csvMan.saveToFile ("testsave.csv");

	//cout << "csvMan.saveToFile (\"testsave.csv\");   test" << endl;
	//cout << "--------------------------------------------" << endl << endl;
}

//静态方法测试
void getRowFromStreamTest()
{
	CXW::CSVManage::Row row;

	fstream inf;
	inf.open ("test.csv");
	if(inf.fail())
	{
		cout << "open file error! " << endl;
		return ;
	}

	row = CXW::CSVManage::getRowFromStream (inf);
	while (0 != row.size ())
	{
		cout << "getRowFromStream success ! " << " row size : " << row.size ()  
			<< "   row[0] = [" << row[0] << "]"
			<< "  row[" << row.size ()-1 << "] = [" << row[row.size ()-1] << "]" << endl;

		row = CXW::CSVManage::getRowFromStream (inf);
	}
}

//静态方法测试
void addRowToStreamTest()
{
	fstream inf;
	inf.open ("addRowtoStreamTest.csv",ios_base::app );

	//如果文件不存在，则创建并添加标题行
	if(inf.fail())
	{
		cout << "file does not exist,create it! " << endl;
		inf.open ("addRowtoStreamTest.csv",ios_base::out);
		CXW::CSVManage::Title title;
		title.push_back ("编号");
		title.push_back ("测点名称");
		title.push_back ("时间戳");
		title.push_back ("数值");
		title.push_back ("质量码");

		CXW::CSVManage::addRowToStream (inf,title);
	}
	
	CXW::CSVManage::Row row;
	char buf[32];

	for (int i = 0 ; i < 10000 ; i++)
	{
		row.clear ();

		row.push_back (_itoa (i,buf,10));
		row.push_back ("TagName001");
		row.push_back ("2012-7-12 9:23:00");
		row.push_back (_itoa (rand(),buf,10));
		row.push_back (_itoa (i,buf,16));

		CXW::CSVManage::addRowToStream (inf,row);
	}
	inf.close();
}

void sortTest(CXW::CSVManage& csvMan)
{
	csvMan.sort (1,CXW::CSVManage::STRING_A_TO_Z);
	csvMan.saveToFile ("./test/test_STRING_A_TO_Z.csv");

	csvMan.sort (1,CXW::CSVManage::STRING_Z_TO_A);
	csvMan.saveToFile ("./test/test_STRING_Z_TO_A.csv");

	csvMan.sort (1,CXW::CSVManage::STRING_Aa_TO_Zz);
	csvMan.saveToFile ("./test/test_STRING_Aa_TO_Zz.csv");

	csvMan.sort (1,CXW::CSVManage::STRING_Zz_TO_Aa);
	csvMan.saveToFile ("./test/test_STRING_Zz_TO_Aa.csv");

	csvMan.sort (1,CXW::CSVManage::INT_LITTLE_TO_LARGE);
	csvMan.saveToFile ("./test/test_INT_LITTLE_TO_LARGE.csv");

	csvMan.sort (1,CXW::CSVManage::INT_LARGE_TO_LITTLE);
	csvMan.saveToFile ("./test/test_INT_LARGE_TO_LITTLE.csv");

	csvMan.sort (1,CXW::CSVManage::FLOAT_LITTLE_TO_LARGE);
	csvMan.saveToFile ("./test/test_FLOAT_LITTLE_TO_LARGE.csv");

	csvMan.sort (1,CXW::CSVManage::FLOAT_LARGE_TO_LITTLE);
	csvMan.saveToFile ("./test/test_FLOAT_LARGE_TO_LITTLE.csv");

	csvMan.sort (1,CXW::CSVManage::DOUBLE_LITTLE_TO_LARGE);
	csvMan.saveToFile ("./test/test_DOUBLE_LITTLE_TO_LARGE.csv");

	csvMan.sort (1,CXW::CSVManage::DOUBLE_LARGE_TO_LITTLE);
	csvMan.saveToFile ("./test/test_DOUBLE_LARGE_TO_LITTLE.csv");
}