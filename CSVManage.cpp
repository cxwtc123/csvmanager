/*! 
*  \file	  CSVManage.cpp
*  \brief     CSV文件管理类 源文件
*  \details   CSV文件管理类 源文件，包含CSVManage类的实现
*  \author    cxw
*  \version   1.1
*  \date      2012-7-15
*  \bug       报告BUG，请联系 cxwtc123@gmail.com
*  \copyright free
*/

#include "CSVManage.h"

#include <fstream>

namespace CXW
{

	CSVManage::CSVManage(void)
	{
		hastitle_ = 0;
	}

	CSVManage::~CSVManage(void)
	{
		clear ();
	}


	/************************************************************************/
	/* private方法
	*/
	/************************************************************************/

	//清空数据
	void 
		CSVManage::clear()
	{
		data_.clear ();
		title_.clear ();
		hastitle_ = false;
		datasize_.row = 0;
		datasize_.col = 0;
		dataloca_.row = 0;
		dataloca_.col = 0;
	}

	//设定数据规模
	void 
		CSVManage::setDataSize(int rowSize,int colSize)
	{
		datasize_.row = rowSize;
		datasize_.col = colSize;
	}

	void 
		CSVManage::setDataSize(DataSize dataSize)
	{
		setDataSize(dataSize.row,dataSize.col);
	}

	/* static方法
	*/
	/************************************************************************/

	//判断文件是否合法
	int
		CSVManage::isFileLegal(const char* filename)
	{
		//判断文件名是否合法
		if (NULL == filename)
		{
			return FILE_NAME_NULL;
		}

		//判断文件是否正确打开
		//ofstream out;
		//out.open (filename);
		//if (out.bad())
		//{
		//	return FILE_CAN_NOT_OPEN;
		//}

		//字符编码
		//{
		//}
		return SUCCESS;
	}


	//如果字符串含有逗号，则添加双引号以避免错误
	string 
		CSVManage::toCSVString(const char* str)
	{
		int needFlag = 0;
		string string_ = str;
		unsigned int strLenth = string_.size();

		//如果是空字符串
		//add for [BUG8]
		if (strLenth < 1)
		{
			return string_;
		}

		//如果字段前后包含有空格，该字段用双引号括起来,,,
		//modify 2012-7-9，增加对 制表符 的判断
		if (str[0] == ' ' || str[strLenth - 1] == ' '
			|| str[0] == '\t' || str[strLenth - 1] == '\t')
		{
			needFlag = 1;
		}

		for (unsigned int i = 0 ; i < strLenth ; i++)
		{
			//字段中包含有逗号，该字段用双引号括起来,,,
			//字段中包含有换行符，该字段用双引号括起来,,,
			//字段中如果有双引号，该字段用双引号括起来,,,
			if (',' == str[i] || '\n' == str[i] || '\"' == str[i])
			{
				needFlag = 1;
				break;
			}
			else
			{
				continue;
			}
		}

		if (needFlag)
		{
			string_ = "\"";
			for (unsigned int j = 0 ; j < strLenth ; j++)
			{
				//字段中的双引号用两个双引号表示
				if ('\"' == str[j])
				{
					string_ += "\"";
				}
				string_ += str[j];
			}
			string_ += "\"";
		}
		else
		{
			string_ = str;
		}
		return string_;
	}

	//对列进行排序
	//不改变序列顺序，只记录排序结果
	template<typename T>
	void CSVManage::sortCol(const vector<T>& col,SeqSig &seqsig,bool li2la)
	{
		if (! col.size ())
		{
			return;
		}
		seqsig.resize (col.size ());
		for (unsigned int i = 0; i < seqsig.size () ; i++)
		{
			seqsig[i] = i;
		}

		int temseqflag;
		//如果是正向排序 从小到大
		if (li2la)
		{
			for (unsigned int i = 0; i < col.size () - 1; i++)
			{
				for (unsigned int j = 0 ; j < col.size() - 1 - i; j++)
				{
					if (col[seqsig[j]] > col[seqsig[j+1]])
					{
						temseqflag = seqsig[j];
						seqsig[j] = seqsig[j+1];
						seqsig[j+1] = temseqflag;					
					}
				}
			}
		}
		//否则，逆向排序  从大到小
		else
		{
			for (unsigned int i = 0; i < col.size () - 1; i++)
			{
				for (unsigned int j = 0 ; j < col.size() - 1 - i; j++)
				{
					if (col[seqsig[j]] < col[seqsig[j+1]])
					{
						temseqflag = seqsig[j];
						seqsig[j] = seqsig[j+1];
						seqsig[j+1] = temseqflag;					
					}
				}
			}
		}
	}

	//从字符流获取一行内容
	//这部分代码可以和 loadFile 部分代码复用___
	CSVManage::Row 
		CSVManage::getRowFromStream(fstream& inf)
	{
		Row row;
		char ch;
		string xy = "";

		inf.get (ch);

		while (inf)
		{
			//每次完成一个元素数据的读取
			xy = "";

			//忽略元素前部的 空格 和 制表符
			while ('\t' == ch || ' ' == ch)
			{
				inf.get(ch);
				continue;
			}
			char next = 0;
			int yinhao = 0;//双引号个数

			//如果第一个字符是 " ，说明本元素包含： 逗号、换行符、首尾空格、双引号等
			if (inf && ch == '\"')
			{
				yinhao++ ;
				string tempxy = "";
				inf.get (ch);
				//直到遇到下一个逗号或者换行符。逗号或换行符需要满足： 之前有偶数个双引号
				while (inf && !((ch ==',' || ch == '\n') && yinhao%2 == 0))
				{
					xy += ch;
					//双引号 数目累加
					if (ch == '\"')
					{
						yinhao++ ;
					}
					inf.get (ch);
				}
				//把元素末尾的 空格 和 制表符滤除 2012-7-9，chenxw 
				while (xy.size() && 
					(xy[xy.size() - 1] == ' '|| xy[xy.size() - 1] == '\t'))
				{
					xy.resize(xy.size() - 1);
				}

				//去除最后一个双引号
				//对于标准 csv，该元素的在 csv 文本中的最后一个字符应当是 双引号
				//为了兼容非标准 csv ，如果最后一个双引号之后仍然有字符，视为有效
				int yinhaolocation = 0;
				//记下引号位置
				for (unsigned int i = xy.size () - 1 ; i > 0 ; i--)
				{
					if (xy[i] == '\"')
					{
						yinhaolocation = i;
						break;
					}						
				}
				//去除最后一个引号
				for (unsigned int i = 0 ; i < xy.size () ; i++)
				{
					if (i != yinhaolocation)
					{
						tempxy.push_back(xy[i]);
					}
				}

				//xy初始化
				xy.clear();
				//中间的双引号，应该都是两两连续的
				for (unsigned int i = 0 ; i < tempxy.size (); i++)
				{
					//如果有两个连续的双引号，则去除该字符串
					if ( i < tempxy.size() - 1 && tempxy[i] == '\"' && tempxy[i+1] == '\"')
					{
						i ++;
					}
					xy.push_back(tempxy[i]);
				}
			}
			else
			{
				//第一个字符不是 " 的处理方式相同
				while(inf &&	(ch != ',') && (ch != '\n'))
				{
					xy += ch;
					inf.get (ch);//
				}
				// add for [BUG1]
				//把元素末尾的 空格 和 制表符滤除 2012-7-9，chenxw 
				while (xy.size() && 
					(xy[xy.size() - 1] == ' '|| xy[xy.size() - 1] == '\t'))
				{
					xy.resize(xy.size() - 1);
				}
			}

			row.push_back (xy);
			//如果读到换行符，退出循环
			if (ch == '\n')
			{
				break;
			}
			inf.get (ch);
		}
		//如果最后一个字符是 ',' 而不是换行符，说明文件结束且最后一个元素为空
		if (ch == ',')
		{	
			row.push_back ("");
		}
		return row;
	}

	//向文件流写入一行内容
	bool 
		CSVManage::addRowToStream(fstream& outf,const Row& row)
	{
		if (outf.fail ())
		{
			return false;
		}
		//add for Debug [BUG6]
		if (row.size () < 1)
		{
			return false;
		}

		//每一行的第 0 ~ 第 datasize_.col - 1 个字符串
		for (unsigned int j = 0 ; j < row.size () - 1; j++)
		{
			outf <<  toCSVString(row[j].c_str()) << ",";
		}
		//每一行最后一个字符串
		outf << toCSVString(row[row.size () - 1].c_str()) << endl;
		return true;
	}



	/* public方法
	*/
	/************************************************************************/

	//是否有标题行
	bool 
		CSVManage::hasTitle() const
	{
		return hastitle_;
	}

	//获取标题行
	CSVManage::Title 
		CSVManage::getTitle() const
	{
		Title title;
		if (hastitle_)
		{
			return title_;
		}
		else
		{
			return title;
		}
	}

	//删除标题行
	void 
		CSVManage::delTitle()
	{
		title_.clear ();
		hastitle_ = false;
	}

	//修改标题行
	int
		CSVManage::modifyTitle(Title title)
	{
		if (hastitle_)
		{
			//如果标题行尚不存在或者标题行长度匹配原长度
			if (0 == title_.size () || title.size () == title_.size ())
			{
				title_ = title;
				return SUCCESS;
			}
			else
			{
				return TITLE_LEN_NOT_MATCH;
			}
		}
		else
		{
			return TITLE_NOT_EXIST;
		}
	}

	//获取数据规模
	CSVManage::DataSize
		CSVManage::getDataSize() const
	{
		DataSize datasize;
		datasize.row = datasize_.row ;
		datasize.col = datasize_.col ;
		return datasize;
	}

	//获取行个数
	int
		CSVManage::getRowSize() const
	{
		return datasize_.row;
	}

	//获取列个数
	int 
		CSVManage::getColSize() const
	{
		return datasize_.col;
	}

	//新建文件，默认文件会变成此文件，数据会被清空
	bool 
		CSVManage::newFile(const char* filename,const Title& title)
	{
		ofstream out;
		out.open (filename);
		if(out.fail ())
		{
			return false;
		}
		clear ();
		if (0 != title.size ())
		{
			hastitle_ = true;
			modifyTitle (title);
		}
		//记录默认文件名称
		filename_ = filename;
		save ();
		return true;	
	}


	//载入文件
	bool
		CSVManage::loadFile(const char* filename,bool hastitle)
	{
		if(SUCCESS != isFileLegal(filename))
		{
			return false;
		}

		ifstream in; // Must use open(filename), not ifstream in(filename) (VC++ bug)
		in.open(filename);
		if(in.fail())
			return false;

		clear();

		//标题行已经载入
		bool titleloaded = false;

		hastitle_ = hastitle;
		//如果没有标题行
		if (! hastitle_)
		{
			//不需要载入标题行，置位为 true
			titleloaded = true;
		}

		char ch = 0;
		in.get(ch);

		string xy = "";
		Row row;

		while(in)
		{
			//每次完成一个元素数据的读取

			xy = "";

			//如果是本行第一个数据，去除之前的空行
			if (0 == row.size () && '\n' == ch)
			{
				in.get(ch);
				continue;
			}

			//忽略元素前部的 空格 和 制表符
			while ('\t' == ch || ' ' == ch)
			{
				in.get(ch);
				continue;
			}
			char next = 0;
			int yinhao = 0;//双引号个数

			//如果第一个字符是 " ，说明本元素包含： 逗号、换行符、首尾空格、双引号等
			if (in && ch == '\"')
			{
				yinhao++ ;
				string tempxy = "";
				in.get (ch);
				//直到遇到下一个逗号或者换行符。逗号或换行符需要满足： 之前有偶数个双引号
				while (in && !((ch ==',' || ch == '\n') && yinhao%2 == 0))
				{
					xy += ch;
					//双引号 数目累加
					if (ch == '\"')
					{
						yinhao++ ;
					}
					in.get (ch);
				}
				//把元素末尾的 空格 和 制表符滤除 2012-7-9，chenxw 
				while (xy.size() && 
					(xy[xy.size() - 1] == ' '|| xy[xy.size() - 1] == '\t'))
				{
					xy.resize(xy.size() - 1);
				}

				//去除最后一个双引号
				//对于标准 csv，该元素的在 csv 文本中的最后一个字符应当是 双引号
				//为了兼容非标准 csv ，如果最后一个双引号之后仍然有字符，视为有效
				int yinhaolocation = 0;
				//记下引号位置
				for (unsigned int i = xy.size () - 1 ; i > 0 ; i--)
				{
					if (xy[i] == '\"')
					{
						yinhaolocation = i;
						break;
					}						
				}
				//去除最后一个引号
				for (unsigned int i = 0 ; i < xy.size () ; i++)
				{
					if (i != yinhaolocation)
					{
						tempxy.push_back(xy[i]);
					}
				}

				//xy初始化
				xy.clear();
				//中间的双引号，应该都是两两连续的
				for (unsigned int i = 0 ; i < tempxy.size (); i++)
				{
					//如果有两个连续的双引号，则去除该字符串
					if ( i < tempxy.size() - 1 && tempxy[i] == '\"' && tempxy[i+1] == '\"')
					{
						i ++;
					}
					xy.push_back(tempxy[i]);
				}
			}
			else
			{
				//第一个字符不是 " 的处理方式相同
				while(in &&	(ch != ',') && (ch != '\n'))
				{
					xy += ch;
					in.get (ch);//
				}
				// add for [BUG1]
				//把元素末尾的 空格 和 制表符滤除 2012-7-9，chenxw 
				while (xy.size() && 
					(xy[xy.size() - 1] == ' '|| xy[xy.size() - 1] == '\t'))
				{
					xy.resize(xy.size() - 1);
				}
			}

			row.push_back (xy);
			
			//如果读到换行符，则写入本行数据，并清空行 行缓存 row
			if (ch == '\n')
			{
				if (titleloaded)
				{
					addRow (row);
				}
				else
				{
					title_ = row;
					titleloaded = true;
					setDataSize (0,row.size ());
				}
				row.clear();
			}
			in.get (ch);
		}
		//如果文件结束，仍有未写入的内容，说明最后一行没有换行符
		if(0 != row.size ())
		{
			//最后一个元素是 '，说明最后一行最后一个元素为空
			if (ch == ',')
			{	
				row.push_back ("");
			}
			if (titleloaded)
			{
				addRow (row);
			}
			else
			{
				title_ = row;
				titleloaded = true;
				setDataSize (0,row.size ());
			}
			row.clear();
		}
		in.close();
		filename_ = filename;
		return true;
	}

	//保存到文件
	bool
		CSVManage::saveToFile(const char* filename) const
	{
		ofstream out;
		out.open (filename);
		if(out.fail ())
		{
			return false;
		}
	
		//如果有标题行且标题行长度不为0，则先输出标题行
		//add for Debug [BUG7]
		if (hastitle_ && title_.size () > 0)
		{
			//第 0 ~ 第 datasize_.col - 1 个字符串
			for (unsigned int j = 0 ; j < title_.size () - 1; j++)
			{
				out <<  toCSVString(title_[j].c_str()) << ",";
			}
			//最后一个字符串
			out << toCSVString(title_[title_.size () - 1].c_str()) << endl;
		}

		//如果数据不为空
		//add for Debug [BUG7]
		if (getColSize () > 0 && getRowSize () > 0)
		{
			for (int i = 0 ; i < getRowSize () ; i++)
			{
				//每一行的第 0 ~ 第 datasize_.col - 1 个字符串
				for (int j = 0 ; j < getColSize () - 1; j++)
				{
					out <<  toCSVString(data_[i][j].c_str()) << ",";
				}
				//每一行最后一个字符串
				out << toCSVString(data_[i][getColSize () - 1].c_str()) << endl;
			}
		}

		out.close();
		return true;
	}

	//保存(到默认文件)
	bool 
		CSVManage::save() const
	{
		//如果默认文件名有效
		if (SUCCESS == isFileLegal (filename_.c_str()))
		{
			return saveToFile (filename_.c_str());
		}
		else
		{
			return false;
		}
	}

	//取一行
	CSVManage::Row
		CSVManage::getRow(int rowNum) const
	{
		Row rowRet;
		if (rowNum >=  datasize_.row
			|| rowNum < 0)
		{
			return rowRet;
		}
		else
		{
			return data_[rowNum];
		}
	}

	//取一行，第 colStart 到 第 colEnd 列
	CSVManage::Row 
		CSVManage::getRow(int rowNum,int colStart,int colEnd) const
	{
		Row rowRet;
		//rowNum 非法
		if (rowNum >=  datasize_.row
			|| rowNum < 0)
		{
			return rowRet;
		}

		//colStart、colEnd 非法
		if (colStart >= datasize_.col 
			|| colEnd >= datasize_.col
			|| colStart >= colEnd)
		{
			return rowRet;
		}
		for (int i = colStart ;i < colEnd ; i++)
		{
			rowRet.push_back (data_[rowNum][i]);
		}
		return rowRet;
	}

	//取一列
	CSVManage::Col 
		CSVManage::getCol(int colNum) const
	{
		Col colRet;
		if (colNum >=  datasize_.col
			|| colNum < 0)
		{
			return colRet;
		}
		else
		{
			for (int i = 0 ; i < datasize_.row ; i++)
			{
				colRet.push_back (data_[i][colNum]);
			}
			return colRet;
		}
	}

	//取一列，第 rowStart 到 第 rowEnd 列
	CSVManage::Col 
		CSVManage::getCol(int colNum,int rowStart,int rowEnd) const
	{
		Col colRet;
		//colNum 非法
		if (colNum >=  datasize_.col
			|| colNum < 0)
		{
			return colRet;
		}

		//rowStart、rowEnd 非法
		if (rowStart >= datasize_.row 
			|| rowEnd >= datasize_.row
			|| rowStart >= rowEnd)
		{
			return colRet;
		}
		for (int i = rowStart ;i < rowEnd ; i++)
		{
			colRet.push_back (data_[i][colNum]);
		}
		return colRet;
	}

	//按坐标取值，取某个单元格
	string 
		CSVManage::getXY(int rowNum,int colNum) const
	{
		string xy;
		if (rowNum >=  datasize_.row
			|| rowNum < 0)
		{
			return xy;
		}
		else if (colNum >=  datasize_.col
			|| colNum < 0)
		{
			return xy;
		}
		else
		{
			return data_[rowNum][colNum];
		}
	}

	//对内容按任意列的排序
	void
		CSVManage::sort(int colNum,SORT_MODE sortMode)
	{
		Col col = getCol (colNum);

		//如果列不存在
		if (!col.size ())
		{
			return ;
		}

		SeqSig seqFlag;
		vector<int> vector_i;
		vector<float> vector_f;
		vector<double> vector_d;
		vector<string> vector_s;

		switch (sortMode)
		{
		case STRING_A_TO_Z			:
			sortCol (col,seqFlag,true);
			break;
		case STRING_Z_TO_A			:	
			sortCol (col,seqFlag,false);
			break;
		case STRING_Aa_TO_Zz		:
			vector_s.resize (col.size());
			for (unsigned int i = 0 ; i < vector_s.size () ; i++)
			{
				vector_s[i] = col[i];
				for (unsigned int j = 0; j < vector_s[i].size() ; j++)
				{
					if ( 'Z' >= vector_s[i][j] &&
						'A' <= vector_s[i][j] )
					{
						vector_s[i][j] += 32;	//大写字母转为小写字母 
					}
				}
			}
			sortCol (vector_s,seqFlag,true);
			break;
		case STRING_Zz_TO_Aa		:
			vector_s.resize (col.size());
			for (unsigned int i = 0 ; i < vector_s.size () ; i++)
			{
				vector_s[i] = col[i];
				for (unsigned int j = 0; j < vector_s[i].size() ; j++)
				{
					if ( 'Z' >= vector_s[i][j] &&
						'A' <= vector_s[i][j] )
					{
						vector_s[i][j] += 32;	//大写字母转为小写字母 
					}
				}
			}
			sortCol (vector_s,seqFlag,false);
			break;
		case INT_LITTLE_TO_LARGE	:
			vector_i.resize (col.size());
			for (unsigned int i = 0 ; i < vector_i.size () ; i++)
			{
				vector_i[i] = atoi(col[i].c_str());
			}
			sortCol (vector_i,seqFlag,true);
			break;
		case INT_LARGE_TO_LITTLE	:
			vector_i.resize (col.size());
			for (unsigned int i = 0 ; i < vector_i.size () ; i++)
			{
				vector_i[i] = atoi(col[i].c_str());
			}
			sortCol (vector_i,seqFlag,false);
			break;
		case FLOAT_LITTLE_TO_LARGE	:
			vector_f.resize (col.size());
			for (unsigned int i = 0 ; i < vector_f.size () ; i++)
			{
				vector_f[i] = (float)atof(col[i].c_str());
			}
			sortCol (vector_f,seqFlag,true);
			break;
		case FLOAT_LARGE_TO_LITTLE	:
			vector_f.resize (col.size());
			for (unsigned int i = 0 ; i < vector_f.size () ; i++)
			{
				vector_f[i] = (float)atof(col[i].c_str());
			}
			sortCol (vector_f,seqFlag,false);
			break;
		case DOUBLE_LITTLE_TO_LARGE	:
			vector_d.resize (col.size());
			for (unsigned int i = 0 ; i < vector_d.size () ; i++)
			{
				vector_d[i] = atof(col[i].c_str());
			}
			sortCol (vector_d,seqFlag,true);
			break;
		case DOUBLE_LARGE_TO_LITTLE	:
			vector_d.resize (col.size());
			for (unsigned int i = 0 ; i < vector_d.size () ; i++)
			{
				vector_d[i] = atof(col[i].c_str());
			}
			sortCol (vector_d,seqFlag,false);
			break;
		default:
			return;
			break;
		}

		//根据排序结果，重新排列行
		Data tempdata;
		//这种方式的内存消耗较多，后期可以优化
		for (unsigned int i = 0 ; i < col.size () ; i++)
		{
			tempdata.push_back (getRow(seqFlag[i]));
		}

		data_ = tempdata;
		return;
	}

	//修改某行某列的内容
	void 
		CSVManage::modify(int rowNum,int colNum,const char* str)
	{
		if (rowNum >=  datasize_.row
			|| rowNum < 0)
		{
			return ;
		}
		else if (colNum >=  datasize_.col
			|| colNum < 0)
		{
			return ;
		}
		else
		{
			data_[rowNum][colNum] = str;
		}
		return;
	}

	//增加一行内容
	int  
		CSVManage::addRow(Row row, int rowNum)
	{
		//如果数据为空
// 		if (0 == datasize_.col || 0 == datasize_.row)
		if (0 == datasize_.col)
		{
			setDataSize (0,row.size());
		}
		
		//如果行为空
		if (row.size () < 1)
		{
			return ROW_COL_NUM_ERROR;
		}
		//如果行长度和列个数不匹配
		else if (row.size () != datasize_.col)
		{
			row.resize (datasize_.col);
		}

		//如果rowNum大于 当前最大，则在最后插入
		if (rowNum >= datasize_.row)
		{
			data_.push_back (row);
		}
		else
		{
			//呜呜，此处尚未完成，如采用list管理，可以方便的进行插入操作..
 			data_.push_back (row);
		}
		datasize_.row ++;
		return SUCCESS;
	}

	//删除一行内容
	int  
		CSVManage::deleteRow(int rowNum)
	{
		if (rowNum >= datasize_.row)
		{
			return ROW_DO_NOT_EXIST;
		}
		else if (rowNum < 0)
		{
			return NUM_TOO_LITTLE;
		}
		else
		{
			//先把末行内容拷贝到本行，再删除最后一行
			data_[rowNum] = data_[datasize_.row - 1];
			data_.pop_back ();
			datasize_.row --;
			return SUCCESS;
		}
	}

}// end namespace CXW