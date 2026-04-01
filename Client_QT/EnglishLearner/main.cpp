#include "EnglishLearner.h"
#include <QtWidgets/QApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QMessageBox>
#include <QDebug>

static bool createConnection()
{
    // 使用SQLite
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("english_learning.db");

    if (!db.open()) {
        qDebug() << "数据库连接失败:" << db.lastError().text();
        return false;
    }

    qDebug() << "数据库连接成功！";

    // 创建表
    QSqlQuery query;

    // 用户表
    query.exec("CREATE TABLE IF NOT EXISTS users ("
        "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password TEXT NOT NULL,"
        "total_score INTEGER DEFAULT 0,"
        "study_days INTEGER DEFAULT 0,"
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP)");

    // 词汇表
    query.exec("CREATE TABLE IF NOT EXISTS vocabulary ("
        "word_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "word TEXT NOT NULL,"
        "translation TEXT NOT NULL,"
        "option1 TEXT,"
        "option2 TEXT,"
        "option3 TEXT)");

    // 学习记录表
    query.exec("CREATE TABLE IF NOT EXISTS study_log ("
        "log_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER,"
        "word_id INTEGER,"
        "is_correct INTEGER,"
        "study_time DATETIME DEFAULT CURRENT_TIMESTAMP)");

    // 插入测试数据（如果表为空）
    query.exec("SELECT COUNT(*) FROM vocabulary");
    if (query.next() && query.value(0).toInt() == 0) {
        query.exec("INSERT INTO vocabulary (word, translation, option1, option2, option3) VALUES "
            "('apple', '苹果', '橘子', '香蕉', '葡萄'),"
            "('car', '汽车', '火车', '飞机', '自行车'),"
            "('happy', '快乐的', '悲伤的', '兴奋的', '疲倦的'),"
            "('big', '大的', '小的', '高的', '矮的'),"
            "('run', '跑', '走', '跳', '爬'),"
            "('blue', '蓝色', '红色', '黄色', '绿色'),"
            "('cat', '猫', '狗', '鸟', '鱼'),"
            "('sun', '太阳', '月亮', '星星', '云')");
        qDebug() << "已插入测试数据";
    }

    return true;
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    if (!createConnection()) {
        QMessageBox::critical(nullptr, "错误", "无法连接到数据库！");
        return -1;
    }

    EnglishLearner w;
    w.show();
    return a.exec();
}