# 后台管理与权限说明

## 默认管理员

服务端启动时会自动检查管理员账户，并在数据库中创建或补齐管理员角色绑定：

- 默认用户名：`admin`
- 默认密码：`Admin@123456`
- 默认角色：`ADMIN`
- Web 入口：`/admin`

生产环境建议通过环境变量覆盖默认账户信息：

```powershell
$env:APP_ADMIN_USERNAME="admin"
$env:APP_ADMIN_PASSWORD="ChangeMe@2026"
$env:APP_ADMIN_DISPLAY_NAME="系统管理员"
$env:APP_ADMIN_EMAIL="admin@example.com"
```

如果不希望应用启动时自动创建管理员，可设置：

```powershell
$env:APP_ADMIN_BOOTSTRAP_ENABLED="false"
```

## 权限模型

本次实现保留 `users.role_name` 作为兼容字段，并新增以下权限表：

- `app_role`：角色定义，当前内置 `USER`、`ADMIN`
- `app_permission`：权限点定义
- `user_role`：用户与角色关系
- `role_permission`：角色与权限关系

内置权限点：

| 权限点 | 说明 |
| --- | --- |
| `LEARNING_ACCESS` | 普通学习功能访问 |
| `ADMIN_ACCESS` | 后台入口访问 |
| `USER_READ` | 查看用户列表和概览 |
| `USER_WRITE` | 调整用户状态和角色 |
| `ROLE_READ` | 查看角色权限配置 |
| `CONTENT_IMPORT` | 导入系统词书等运营内容 |

`USER` 角色默认拥有 `LEARNING_ACCESS`；`ADMIN` 角色默认拥有全部内置权限。

## 后台 API

所有后台接口都需要登录，并要求 `ADMIN_ACCESS` 权限：

- `GET /api/v1/admin/overview`：用户概览
- `GET /api/v1/admin/users`：用户列表，支持 `keyword`、`status`、`roleName`、`page`、`size`
- `PUT /api/v1/admin/users/{userId}/status`：启用或禁用用户，参数 `ACTIVE` / `DISABLED`
- `PUT /api/v1/admin/users/{userId}/role`：调整用户角色
- `GET /api/v1/admin/roles`：查看角色和权限点

系统词书导入接口 `POST /api/v1/books/system/import` 已收紧为需要 `CONTENT_IMPORT` 权限。

## 前端使用

1. 启动服务端并确认 Flyway 已执行到 `V11__admin_role_permission_seed.sql`。
2. 使用管理员账户登录 Web。
3. 顶部导航会显示“后台管理”，进入后可查看用户概览、筛选用户、启停账户、调整角色。

为避免误操作，后台禁止管理员禁用自己或移除自己的管理员角色。
