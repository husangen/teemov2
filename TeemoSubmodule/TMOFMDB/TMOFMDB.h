//
//  TMOFMDB.h
//  TeemoV2
//
//  Created by 崔明辉 on 14-4-2.
//  Copyright (c) 2014年 com.duowan.zpc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <FMDatabase.h>
#import <FMDatabaseQueue.h>

#define TMO_FMDB_INSERTFAIL NSIntegerMax

/**
 *  表结构管理器
 */
@interface TMOFMDBTableScheme : NSObject
/**
 *  表格名称
 */
@property (strong, nonatomic) NSString *tableName;
/**
 *  列信息集，NSArray，Array中的每个成员都必须为TMOFMDBColumnScheme实例
 */
@property (strong, nonatomic) NSArray *columnScheme;
@end

typedef enum{
    /**
     *  纯文字
     */
    TMOFMDBColumnTypeText = 0,
    /**
     *  数字、浮点
     */
    TMOFMDBColumnTypeInteger = 1,
    /**
     *  二进制
     */
    TMOFMDBColumnTypeBLOB = 2,
    /**
     *  REAL列
     */
    TMOFMDBColumnTypeReal = 3,
    /**
     *  空值列
     */
    TMOFMDBColumnTypeNull = 4,
} TMOFMDBColumnType;

typedef enum {
    /**
     *  无索引
     */
    TMOFMDBColumnIndexNone = 0,
    /**
     *  主键（一个表只允许有一个主键）
     */
    TMOFMDBColumnIndexPrimaryKey = 1,
    /**
     *  唯一键
     */
    TMOFMDBColumnIndexUnique = 2,
} TMOFMDBColumnIndex;

/**
 *  列结构管理器
 */
@interface TMOFMDBColumnScheme : NSObject

/**
 *  表格列信息
 *
 *  @param argName  列名称
 *  @param argType  列数据类型
 *  @param argIndex 列索引类型
 *
 *  @return TMOFMDBColumnScheme实例
 */
+ (TMOFMDBColumnScheme *)schemeWithName:(NSString *)argName
                                   type:(TMOFMDBColumnType)argType
                                  index:(TMOFMDBColumnIndex)argIndex;
/**
 *  列名称
 */
@property (strong, nonatomic) NSString *columnName;
/**
 *  列数据类型
 */
@property (assign, nonatomic) TMOFMDBColumnType columnType;
/**
 *  列索引类型（主键、唯一键在此设置）
 */
@property (assign, nonatomic) TMOFMDBColumnIndex columnIndex;
@end

@interface FMDatabase (TMOFMDatabase)

/**
 *  使用表、列管理器，创建一个数据表
 *  当程序发现数据库中的表已经存在时，会检查表结构是否一致
 *  若不一致时，会重构整个表，并将数据迁移至新表
 *  SO：你不需要担心应用升级后的表结构变化
 *
 *  @param argScheme 表结构管理器
 */
- (void)createTable:(TMOFMDBTableScheme *)argScheme;

/**
 *  查询一条SQL命令，并返回一行数据（同步）
 *
 *  @param argSql SQL语句
 *
 *  @return 数据结果，找不到数据则返回nil
 */
- (NSDictionary *)findWithSql:(NSString *)argSql,...;

/**
 *  查询一条SQL命令，并以数组形式回调返回多行数据（同步）
 *
 *  @param argSql SQL语句
 *
 *  @return 数据结果，找不到数据则返回nil
 */
- (NSArray *)selectWithSql:(NSString *)argSql,...;

/**
 *  查询一条SQL命令，并以字典形式回调返回一行数据（异步）
 *
 *  @param argCallback 回调方法
 *  @param argSql      SQL语句
 */
- (void)findWithCallback:(void(^)(NSDictionary *result))argCallback withSql:(NSString *)argSql,...;

/**
 *  查询一条SQL命令，并以子线程回调
 *  这是TeemoV2.1提供的新增方法，你应在callback中执行子线程操作（比如要继续执行下一个查询）
 *  你应使用执行UI更新操作（UI更新操作应始终在主线程执行）
 *
 *  @param argCallback           子线程回调
 *  @param argMainThreadCallback 主线程回调
 *  @param argSql                执行SQL语句
 */
- (void)findWithCallbackOnQueryThread:(void (^)(NSDictionary *result, FMDatabase *db))argCallback
                              withSql:(NSString *)argSql, ...;

/**
 *  查询一条SQL命令，并以数组形式回调返回多行数据（异步）
 *
 *  @param argCallback 回调方法
 *  @param argSql      SQL语句
 */
- (void)selectWithCallback:(void(^)(NSArray *result))argCallback withSql:(NSString *)argSql,...;

/**
 *  查询一条SQL命令，并以数组形式回调返回多行数据（异步）
 *  这是TeemoV2.1提供的新增方法，你应在callback中执行子线程操作（比如要继续执行下一个查询）
 *  你应在mainThreadCallback中执行UI更新操作（UI更新操作应始终在主线程执行）
 *
 *  @param argCallback           子线程回调
 *  @param argMainThreadCallback 主线程回调
 *  @param argSql                执行SQL语句
 */
- (void)selectWithCallbackOnQueryThread:(void (^)(NSArray *result, FMDatabase *db))argCallback
                                withSql:(NSString *)argSql, ...;

@end

@interface TMODatabaseQueue : FMDatabaseQueue

- (void)inDatabaseRunOnMainThread:(void (^)(FMDatabase *))block;

@end

@interface TMOFMDB : NSObject

/**
*  获取一个线程安全FMDatabaseQueue实例
*  墙裂建议所有操作都使用线程安全方法
*  除非，你认为数据库是只读的，不作任何事务写入
*
*  @param argPath 传入sqlite数据文件路径
*
*  @return FMDatabaseQueue单例
*/
+ (TMODatabaseQueue *)instanceWithPath:(NSString *)argPath;

/**
 *  获取一个线程安全FMDatabaseQueue实例，由系统指定路径，默认数据表存放于/tmp目录下
 *  不建议放置长期持久化数据
 *  缓存级数据应该放置在Library/Caches目录
 *  用户级数据应该放置在Documents目录下
 *
 *  @return FMDatabaseQueue单例
 */
+ (TMODatabaseQueue *)defaultDatabase;//使用默认的数据表

@end
