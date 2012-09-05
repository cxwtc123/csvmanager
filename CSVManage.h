/*! 
*  \file	  CSVManage.h
*  \brief     CSV文件管理类 头文件
*  \details   CSV文件管理类 头文件，包含CSVManage类的定义，用于管理CSV文件
*  \author    cxw 
*  \version   1.1
*  \date      2012-7-15
*  \bug       报告BUG，请联系 cxwtc123@gmail.com
*  \copyright free
*/


#ifndef _CXW_CSV_MANAGE_H
#define _CXW_CSV_MANAGE_H

#include <iostream>
#include <string>
#include <vector>
// #include <list>


//!  CXW命名空间 
/*!
CXW的C++命名空间
*/
namespace CXW
{
	using namespace std;

	//!  CSV文件管理类. 
	/*!
	1.每个对象对应一个默认文件，新建或者载入文件会更改默认文件名称\n
	2.整个文件内容以二维数组的形式展现，使用 行标 和 列标 访问内容，所有下标从 0 开始\n
	3.支持以指定列内容的特定数据类型进行排序\n
	4.支持标题行，在载入时可以选择是否有标题行，标题行不在数据行范围内\n
	5.支持每个单元格元素内包含 [空格、制表符、双引号、逗号、换行符] 等特殊字符\n
	6.文件末尾的换行符不是必须的\n
	7.以标题行或者第一行内容确定整个文件每行的列数量，其余行的不足部分会被认为是空元素、超出部分会被忽略\n
	8.loadFile 和 newFile 会把CSV的所有内容载入到内存，适用于处理文件大小在 50Mb 以内的文件\n
	9.静态方法 getRowFromStream 和 addRowToStream 为个性化外部调用提供支持\n
	*/
	class CSVManage
	{
	public:
		CSVManage(void);
		~CSVManage(void);

	public:

		//! 错误码枚举类型
		/*! 错误码*/
		enum ERROR_CODE
		{
			SUCCESS					= 0		,	///<成功

			FILE_NAME_NULL			= -1	,	///<文件名为空
			FILE_TOO_LARGE			= -2	,	///<文件过大，超过本程序的处理能力
			FILE_CAN_NOT_FIND		= -3	,	///<文件不存在
			FILE_CAN_NOT_OPEN		= -5	,	///<文件无法打开

			NUM_TOO_LARGE			= -6	,	///<数值过大
			NUM_TOO_LITTLE			= -7	,	///<数值过小

			ROW_DO_NOT_EXIST		= -8	,	///<行不存在
			ROW_COL_NUM_ERROR		= -9	,	///<行的列数量不匹配

			TITLE_NOT_EXIST			= -10	,	///<标题行不存在
			TITLE_LEN_NOT_MATCH		= -11	,	///<标题行长度不匹配
		};

		//! 排序模式枚举类型
		/*! 排序模式*/
		enum SORT_MODE
		{
			STRING_A_TO_Z			= 1		,	///<字符串从 A 到 Z，判断大小写整型数据从小到大
			STRING_Z_TO_A			= 2		,	///<字符串从 Z 到 A，判断大小写整型数据从大到小
												  
			STRING_Aa_TO_Zz			= 3		,	///<字符串从 A 到 Z，忽略大小写单精度浮点型数据从小到大
			STRING_Zz_TO_Aa			= 4		,	///<字符串从 Z 到 A，忽略大小写单精度浮点型数据从大到小

			INT_LITTLE_TO_LARGE		= 5		,	///<整型数据从小到大
			INT_LARGE_TO_LITTLE		= 6		,	///<整型数据从大到小

			FLOAT_LITTLE_TO_LARGE	= 7		,	///<单精度浮点型数据从小到大
			FLOAT_LARGE_TO_LITTLE	= 8		,	///<单精度浮点型数据从大到小

			DOUBLE_LITTLE_TO_LARGE	= 9		,	///<双精度浮点型数据从小到大
			DOUBLE_LARGE_TO_LITTLE	= 10	,	///<双精度浮点型数据从大到小
		};

		typedef vector<string> Row;		///<一行数据

		typedef Row Title;				///<标题行

		typedef vector<string> Col;		///<一列数据

		typedef vector<Row> Data;		///<全部数据

		typedef vector<int> SeqSig;		///<排序标记
 


		//! 数据位置
		/*! 数据位置结构体*/
		struct DataLoca					
		{
			DataLoca(void)
			{
				row = 0;	
				col = 0;
			}
			~DataLoca(void)
			{
			}
			int row;		///<行坐标
			int col;		///<列坐标
		};

		//! 数据规模
		/*! 数据规模结构体*/
		typedef DataLoca DataSize;

	private:

		string filename_;	///< 文件名
		Title title_;		///< 标题行
		Data data_;			///< 数据块
		DataSize datasize_;	///< 数据块大小		这部分信息其实已在data_中包含。
		DataLoca dataloca_;	///< 当前位置		尚未使用
		bool hastitle_;		///< 是否包含标题行
		
		/*!清空数据*/
		void clear();

		/*!设定数据规模*/
		void setDataSize(
			int rowSize,
			int colSize);

		/*!设定数据规模*/
		void setDataSize(
			DataSize dataSize
			);

	public:

		/**
		* @brief 判断文件名是否合法
		*
		* isFileLegal()判断文件名是否合法
		*
		* @return int as ERROR_CODE:\n
		*  FILE_NAME_NULL		\n
		*  FILE_CAN_NOT_OPEN	\n
		*  SUCCESS				\n
		* @see ERROR_CODE
		*/
		static int isFileLegal(
			const char* filename	/**< [in] 文件名 */
			);

		/**
		* @brief 元素内容转为用于输出到CSV文件的格式
		*
		* toCSVString()元素内容转为用于输出到CSV文件的格式
		*
		* @return 转换后的字符串
		*/
		static string toCSVString(
			const char* str			/**< [in] 源字符串 */
			);

		/**
		* @brief 模板方法，对数组进行排序
		*
		* sortCol()对类型为T的vector进行排序，输出排序后的顺序编号seqsig
		*
		* @return 空
		*/
		template<typename T>
		static void sortCol(
			const vector<T>& col,	/**< [in] 排序的数组 */
			SeqSig &seqsig,			/**< [out] 输出排序后的顺序编号 */
			bool li2la = true		/**< [in] 是否从小到大排序 */
			);

		/**
		* @brief 从文件流获取一行内容
		*
		* getRowFromStream() 从文件流获取一行内容
		*
		* @return 本行内容，如果行不存在则返回空行
		*/
		static Row getRowFromStream(
			fstream& inf			/**< [inout] 文件流 */
			);

		/**
		* @brief 向文件流写入一行内容
		*
		* addRowToStream() 向文件流写入一行内容
		*
		* @return 写入是否成功
		*/
		static bool addRowToStream(
			fstream& outf,			/**< [inout] 文件流 */
			const Row& row			/**< [in] 需要写入的行 */
			);
		
		/**
		* @brief 判断是否有标题行
		*
		* hasTitle()  判断是否有标题行
		*
		* @return  是否有标题行
		*/
		bool hasTitle() const;

		/**
		* @brief 获取标题行
		*
		* getTitle()  获取标题行如果没有标题行，返回空Title
		*
		* @return  标题行内容
		*/
		Title getTitle() const;

		/**
		* @brief 删除标题行
		*
		* delTitle()  删除标题行
		*
		* @return  空
		*/
		void delTitle();


		/**
		* @brief 修改标题行
		*
		* modifyTitle()  修改标题行，只有当标题行已存在是才能修改，并且标题行长度必须和原长度保持一致
		*
		* @return  标题行内容
		*/
		int modifyTitle(
			Title title		/**< [in]  标题行内容*/
			);

		/**
		* @brief 获取数据规模
		*
		* getDataSize()  获取数据规模
		*
		* @return  数据规模 DataSize
		*/
		DataSize getDataSize() const;

		/**
		* @brief 获取行个数
		*
		* getRowSize()  获取行个数
		*
		* @return  行个数
		*/
		int getRowSize() const;

		/**
		* @brief 获取列个数
		*
		* getColSize()  获取列个数
		*
		* @return  列个数
		*/
		int getColSize() const;

		/**
		* @brief 新建文件
		*
		* newFile()  新建文件，默认文件会变成此文件，数据会被清空
		*
		* @return  是否成功
		*/
		bool newFile(
			const char* filename,		/**< [in] 文件名 */
			const Title& title			/**< [inout] 标题行，如果为空，则没有标题行 */
			);

		/**
		* @brief 载入文件
		*
		* loadFile()  载入文件名为filename的文件
		*
		* @return  是否成功
		*/
		bool loadFile(
			const char* filename,	/**< [in] 文件名 */
			bool hastitle = false	/**< [in] 是否有标题行 */
			);

		/**
		* @brief 保存到文件
		*
		* saveToFile()  保存到文件
		*
		* @return  是否成功
		*/
		bool saveToFile(
			const char* filename	/**< [in] 文件名 */
			) const;

		/**
		* @brief 保存到默认文件
		*
		* save()  保存到默认文件
		*
		* @return  是否成功
		*/
		bool save() const;

		/**
		* @brief 取一行内容
		*
		* getRow()  取一行内容，如果内容不存在，返回空行
		*
		* @return  指定行内容
		*/
		Row getRow(
			int rowNum	/**< [in] 行号 */
			) const;

		/**
		* @brief 取一行内容
		*
		* getRow()  取一行内容，如果内容不存在，返回空行
		*
		* @return  指定行内容
		*/
		Row getRow(
			int rowNum,		/**< [in] 行号 */
			int colStart,	/**< [in] 列的起始编号 */
			int colEnd		/**< [in] 列的终止编号 */
			) const;

		/**
		* @brief 取一列内容
		*
		* getCol()  取一列内容，如果内容不存在，返回空列
		*
		* @return  指定列内容
		*/
		Col getCol(
			int colNum		/**< [in] 列号 */
			) const;

		/**
		* @brief 取一列内容
		*
		* getCol()  取一列内容，如果内容不存在，返回空列
		*
		* @return  指定列内容
		*/		
		Col getCol(
			int colNum,		/**< [in] 列号 */
			int rowStart	/**< [in] 行的起始编号 */,
			int rowEnd		/**< [in] 行的终止编号 */
			) const;

		/**
		* @brief 取某个单元格的内容
		*
		* getXY()  按坐标取值，取某个单元格的内容
		*
		* @return 指定单元格的内容
		*/
		string getXY(
			int rowNum,	/**< [in] 行的序号 */
			int colNum	/**< [in] 列的序号 */
			) const;

		/**
		* @brief 对内容按任意列的排序
		*
		* sort() 对整个文件的内容按照第colNum列的sortMode类型进行排序
		*
		* @return 空
		*/	
		void sort(int colNum,					/**< [in] 列的序号 */
			SORT_MODE sortMode = STRING_A_TO_Z	/**< [in] 排序类型 */
			);

		/**
		* @brief 修改某行某列的内容
		*
		* modify()修改某行某列的内容为指定的字符串
		*
		* @return 空
		*/
		void modify(int rowNum,	/**< [in] 行的序号 */
			int colNum,			/**< [in] 列的序号 */
			const char* str		/**< [in] 输入字符串 */
			);

		/**
		* @brief 增加一行内容
		*
		* addRow() 增加一行内容到指定行位置
		*
		* @return int as ERROR_CODE:\n
		*	ROW_COL_NUM_ERROR \n
		*	SUCCESS \n
		* @see ERROR_CODE
		*/
		int addRow(
			Row row,					/**< [in] 行的内容 */
			int rowNum = 0x7fffffff		/**< [in] 行的序号，缺省参数为极大值，将添加行至文件末尾 */
			);

		/**
		* @brief 删除一行内容
		*
		* deleteRow() 删除指定行
		*
		* @return int as ERROR_CODE:\n
		*	ROW_COL_NUM_ERROR \n
		*	SUCCESS \n
		* @see ERROR_CODE
		*/
		int deleteRow(
			int rowNum	/**< [in] 行的序号 */
			);
	};
}// end namespace CXW


#endif