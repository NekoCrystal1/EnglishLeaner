# Database 文档（迁移与变更规范）

目的
记录数据库架构与变更流程，确保后续修改可控、可回滚并易于在本地与 CI 中验证。

位置与迁移目录
- 建议将迁移脚本放在：src/main/resources/db/migration 或 项目根的 db/migration
- 本文档位置：Database.md（当前文件）或 docs/Database.md（若创建 docs 目录）

工具与建议
- 推荐：Flyway（Gradle 插件）或 Liquibase
- Flyway 示例（Gradle）：

plugins {
  id 'org.flywaydb.flyway' version '9.0.0'
}

flyway {
  url = System.getenv('DB_URL') ?: 'jdbc:postgresql://localhost:5432/yourdb'
  user = System.getenv('DB_USER')
  password = System.getenv('DB_PASS')
  locations = ['filesystem:src/main/resources/db/migration']
}

在 Gradle 中运行迁移：
  ./gradlew flywayMigrate

迁移脚本命名规范
- 使用 Flyway 风格：V{version}__description.sql（例如 V1__init.sql）
- 每个脚本说明变更目的，并在需要时提供回滚或备注。

迁移模板（示例）
-- V2__add_user_email.sql
ALTER TABLE users ADD COLUMN email VARCHAR(255);

非破坏性变更流程（新增 NOT NULL 字段示例）
1) V2：新增可空字段（ADD COLUMN ... NULL）
2) 部署新代码并开始写入/读取新字段
3) 回填数据（单独脚本）
4) V3：将字段设为 NOT NULL

回滚策略
- Flyway 不自动回滚；对关键变更准备显式回滚脚本或依赖数据库备份
- 在生产变更前在临时环境验证回滚

在本地与 CI 中测试迁移
- 使用 Docker 启动测试 DB（示例：Postgres）
  docker run --name pg-test -e POSTGRES_PASSWORD=pass -e POSTGRES_USER=user -p 5432:5432 -d postgres:14
- CI 步骤建议：启动测试 DB -> ./gradlew flywayMigrate -> 运行集成测试

示例回填脚本
-- V3__backfill_user_email.sql
UPDATE users SET email = CONCAT(username, '@example.com') WHERE email IS NULL;

注意事项
- 避免在单次迁移中做多项破坏性变更
- 对线上影响大的变更拆分为多次迁移并做灰度验证
- 保留迁移脚本历史，禁止直接在生产 DB 手动修改表结构

后续工作建议
- 补充 ER 图与表字段说明（可以放入 docs/ folder）
- 在代码中使用 Repository/DAO 接口化持久层，便于演进
- 在 CI 中持续执行迁移与集成测试

（此文档为初稿，必要时可扩展为 docs/Database.md）