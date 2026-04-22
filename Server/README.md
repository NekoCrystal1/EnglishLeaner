后端与数据库启动说明

前置环境：
- 安装 Docker Desktop（Windows）并启用 WSL2 或 Hyper-V。
- 安装 JDK 17。
- 安装 Maven（或使用 Maven wrapper）。

启动 SQL Server（开发用示例）：

```bash
cd Server
docker-compose up -d
```

准备后端工程（Gradle）：

```bash
cd Server/backend
./gradlew bootRun    # Linux / macOS
# 或在 Windows 上使用
gradlew.bat bootRun
```

默认后端端口：`8080`。

应用配置：请根据本机 SQL Server 密码修改 `src/main/resources/application.properties` 中的 `spring.datasource.password`。

后续步骤建议：
- 在 `Server/backend` 中初始化 Spring Boot 控制器、实体、仓库。
- 在数据库设计完成后，将表结构同步或使用 Flyway/Liquibase 管理迁移。