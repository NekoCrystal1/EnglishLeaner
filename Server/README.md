# EnglishLearner Server

## 目录说明

- `src/main/java`：后端代码（Spring Boot）
- `src/main/resources/db/migration`：Flyway 数据库迁移脚本
- `src/main/resources/application.yml`：运行配置（默认 H2，可切换 SQL Server）

## 已实现接口

- `POST /api/v1/auth/register`：注册
- `POST /api/v1/auth/login`：登录，返回 JWT
- `GET /api/v1/users/me`：当前登录用户信息
- `GET /api/v1/quiz/next`：获取下一题
- `POST /api/v1/quiz/submit`：提交答案，写入单词学习事件、积分流水和每日汇总
- `GET /api/v1/ranking/top?limit=10`：排行榜
- `GET /api/v1/books?bookType=WORD`：查询系统词书和当前用户上传书籍
- `POST /api/v1/books`：创建用户上传书籍元数据，服务端不保存正文
- `GET /api/v1/books/{bookId}/words`：查询词书单词
- `GET /api/v1/word-learning/plans/current`：获取或创建当前单词计划
- `POST /api/v1/word-learning/plans`：创建单词计划
- `PUT /api/v1/word-learning/plans/{planId}`：更新单词计划
- `GET /api/v1/word-learning/due-words`：获取今日新学/复习单词
- `POST /api/v1/word-learning/events`：提交单词学习事件
- `GET /api/v1/word-learning/summary/today`：查询今日学习汇总

## 数据库可扩展设计

现有后端保留 MVP 旧表（`users`、`vocabulary`、`study_log`、`sync_task_log`），并新增目标设计中的核心表：

- `user_auth_identity`、`user_profile`、`user_device`：账号扩展、画像和设备。
- `book`、`vocabulary_word`、`book_word`：书籍/词书和单词库。
- `user_word_plan`、`user_word_state`、`word_study_event`：单词计划、掌握状态和学习事件。
- `score_transaction`、`daily_study_summary`：积分流水和每日统计。

所有核心表预留以下可修改空间：

- `ext_json`：用于新增需求的弹性字段
- `remark`：运营或后台备注
- `version`：乐观锁版本号
- `is_deleted`：逻辑删除标记

新增字段或表时，统一新增 Flyway 迁移脚本（例如 `V3__xxx.sql`），避免直接手工改库。

当前新增迁移：

- `V3__expand_user_auth.sql`：扩展用户认证、画像、设备、邮箱验证、通知记录。
- `V4__content_book_schema.sql`：新增书籍/词书/单词表，并迁移旧 `vocabulary` 测试数据。
- `V5__word_learning_schema.sql`：新增单词计划、单词状态、学习事件、积分流水和每日汇总。

## 本地启动

1. 安装 JDK 17
2. 安装 Gradle（建议 8.x）
3. 使用 Gradle 启动：
   - `gradle bootRun`
4. 默认端口：`8080`

## 切换 SQL Server

1. 设置环境变量：`DB_HOST`、`DB_PORT`、`DB_NAME`、`DB_USER`、`DB_PASSWORD`
2. 使用 profile：
   - `gradle bootRun --args='--spring.profiles.active=sqlserver'`

## 邮箱登录与发信接入

注册接口支持传入 `email`，登录接口支持用 `account` 传用户名或邮箱。后续接入真实邮件服务时建议使用以下环境变量：

- `MAIL_HOST`
- `MAIL_PORT`
- `MAIL_USERNAME`
- `MAIL_PASSWORD`
- `MAIL_FROM`

验证码流程建议写入 `email_verification`，只保存验证码哈希；发送记录写入 `notification_message`。校验成功后更新 `users.email_verified = 1`。
