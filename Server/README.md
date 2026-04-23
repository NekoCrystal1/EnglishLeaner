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
- `POST /api/v1/quiz/submit`：提交答案并更新积分
- `GET /api/v1/ranking/top?limit=10`：排行榜

## 数据库可扩展设计

所有核心表（`users`、`vocabulary`、`study_log`、`sync_task_log`）预留以下可修改空间：

- `ext_json`：用于新增需求的弹性字段
- `remark`：运营或后台备注
- `version`：乐观锁版本号
- `is_deleted`：逻辑删除标记

新增字段或表时，统一新增 Flyway 迁移脚本（例如 `V3__xxx.sql`），避免直接手工改库。

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
