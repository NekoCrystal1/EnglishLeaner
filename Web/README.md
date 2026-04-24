# EnglishLearner Web 端文档

> 本文档根据项目总说明、Qt 客户端需求文档、服务端数据库设计和当前 Web 端实现编写。  
> 适用范围：EnglishLearner Web 学习端与后续管理端扩展。

## 1. Web 端定位

EnglishLearner 采用 C/S + B/S 混合架构。Qt 客户端承担离线优先的完整学习体验，Web 端承担浏览器轻量入口、在线学习、学习数据查看、排行榜展示和后续管理后台能力。

Web 端第一阶段定位为在线学习 MVP：

- 提供账号注册、登录和 JWT 登录态保持。
- 提供学习概览、积分、学习天数和排名预览。
- 提供在线刷题练习，提交答案后即时反馈正确性和积分变化。
- 提供积分排行榜列表与 ECharts 图表展示。
- 复用后端 RESTful API，不在浏览器端实现 SQLite 离线库。

后续 Web 端可扩展为两类入口：

- 学习端：单词学习、题库练习、阅读、听力、口语、错题、收藏、打卡、成就、学习小组。
- 管理端：用户管理、内容管理、词书/题库/多媒体资源管理、学习数据分析、权限、日志和系统配置。

## 2. 技术栈

| 类型 | 技术 | 说明 |
| --- | --- | --- |
| 前端框架 | Vue 3 | 使用 Composition API 组织页面逻辑 |
| 构建工具 | Vite | 开发热更新和生产构建 |
| UI 组件 | Element Plus | 表单、表格、卡片、导航、提示反馈 |
| 状态管理 | Pinia | 管理登录态、用户信息和全局状态 |
| 路由 | Vue Router | 单页应用路由和登录守卫 |
| HTTP | Axios | 统一请求、Token 注入和错误处理 |
| 图表 | ECharts | 排行榜、趋势图、雷达图、热力图等数据可视化 |

当前 `package.json` 已包含以上依赖。

## 3. 当前目录结构

```text
Web/
  index.html
  package.json
  vite.config.js
  .env.example
  src/
    main.js
    App.vue
    styles.css
    api/
      http.js
      modules.js
    layouts/
      MainLayout.vue
    pages/
      AuthPage.vue
      HomePage.vue
      QuizPage.vue
      RankingPage.vue
    router/
      index.js
    stores/
      auth.js
```

目录职责：

| 目录/文件 | 职责 |
| --- | --- |
| `src/api/http.js` | Axios 实例、基础地址、Token 注入、统一响应解析、401 跳转 |
| `src/api/modules.js` | 按业务模块封装后端接口 |
| `src/stores/auth.js` | 登录态、用户信息、Token 持久化和退出登录 |
| `src/router/index.js` | 页面路由、懒加载和认证守卫 |
| `src/layouts/MainLayout.vue` | 登录后主框架、顶部导航和用户菜单 |
| `src/pages/AuthPage.vue` | 登录/注册页面 |
| `src/pages/HomePage.vue` | 学习概览页面 |
| `src/pages/QuizPage.vue` | 刷题练习页面 |
| `src/pages/RankingPage.vue` | 排行榜页面 |

## 4. 运行与构建

安装依赖：

```bash
npm install
```

开发运行：

```bash
npm run dev
```

生产构建：

```bash
npm run build
```

本地预览构建结果：

```bash
npm run preview
```

后端地址配置：

```bash
cp .env.example .env
```

`VITE_API_BASE_URL` 可配置为完整 API 前缀，例如：

```env
VITE_API_BASE_URL=http://localhost:8080/api/v1
```

如果未配置 `VITE_API_BASE_URL`，前端默认请求 `/api/v1`，开发环境由 `vite.config.js` 中的 `/api` 代理转发到后端。当前代理目标为 `http://localhost:18080`，实际联调时应与后端启动端口保持一致。

## 5. 总体前端架构

Web 端采用典型 SPA 分层：

```text
页面组件
  -> 业务 API 模块
    -> Axios HTTP 实例
      -> Spring Boot REST API
        -> SQL Server 主库
```

登录态流转：

1. 用户在 `AuthPage` 登录或注册。
2. 前端调用 `/auth/login` 或 `/auth/register`。
3. 登录成功后，`auth` store 保存 `token` 和 `user`。
4. `http` 请求拦截器从 `localStorage` 读取 token，并写入 `Authorization: Bearer <token>`。
5. 路由守卫检查 `requiresAuth`，未登录时跳转 `/auth`。
6. 后端返回 401 时，前端清理本地登录态并跳转登录页。

本地持久化：

| Key | 内容 |
| --- | --- |
| `el_token` | JWT Token |
| `el_user` | 当前用户摘要信息 |

浏览器端不保存完整学习离线数据；离线优先、SQLite 子集和本地同步队列属于 Qt 客户端职责。Web 端只保留必要登录态和少量 UI 状态。

## 6. 当前路由

| 路由 | 页面 | 权限 | 说明 |
| --- | --- | --- | --- |
| `/auth` | `AuthPage` | 公开 | 登录、注册 |
| `/` | `HomePage` | 需登录 | 学习概览、个人信息、Top 3 预览 |
| `/quiz` | `QuizPage` | 需登录 | 在线刷题、提交答案、积分反馈 |
| `/ranking` | `RankingPage` | 需登录 | Top 10/20/50 排行榜和柱状图 |
| `/:pathMatch(.*)*` | 重定向 | - | 重定向到 `/` |

后续扩展建议：

| 路由 | 页面方向 | 说明 |
| --- | --- | --- |
| `/words` | 单词学习 | 词书选择、学习计划、复习队列 |
| `/wrong-answers` | 错题集 | 按模块筛选错题并复习 |
| `/favorites` | 收藏夹 | 单词、题目、阅读、听力收藏 |
| `/reading` | 阅读 | 文章列表、阅读详情、阅读题 |
| `/listening` | 听力 | 听力材料、播放器、听力题 |
| `/speaking` | 口语 | 口语任务、录音、评测结果 |
| `/profile` | 我的 | 用户资料、偏好、设备和同步状态 |
| `/admin` | 管理端 | 管理员专用入口 |

## 7. 后端接口约定

当前 Web 端已封装接口：

| 模块 | 方法 | 路径 | 说明 |
| --- | --- | --- | --- |
| 认证 | `POST` | `/api/v1/auth/register` | 用户注册 |
| 认证 | `POST` | `/api/v1/auth/login` | 用户登录 |
| 用户 | `GET` | `/api/v1/users/me` | 获取当前用户信息 |
| 刷题 | `GET` | `/api/v1/quiz/next` | 获取下一题 |
| 刷题 | `POST` | `/api/v1/quiz/submit` | 提交答案 |
| 排行榜 | `GET` | `/api/v1/ranking/top` | 获取排行榜 |

`src/api/http.js` 兼容后端统一响应格式：

```json
{
  "success": true,
  "data": {},
  "message": "ok"
}
```

当 `success = true` 时，页面拿到 `data`；当 `success = false` 时，前端抛出 `message`。如果后端直接返回普通 JSON，前端会直接使用响应体。

建议后续统一接口规范：

| 字段 | 说明 |
| --- | --- |
| `success` | 请求是否成功 |
| `code` | 业务错误码，可选 |
| `message` | 用户可读错误信息 |
| `data` | 业务数据 |
| `traceId` | 排查问题用链路 ID，可选 |

## 8. 页面与数据映射

### 8.1 登录注册

当前能力：

- 用户名/密码登录。
- 用户名/密码注册。
- 注册后自动登录。
- Token 持久化。
- 登录态失效自动回登录页。

后端表映射：

| 前端能力 | 后端表 |
| --- | --- |
| 账号基础信息 | `users` |
| 多方式登录绑定 | `user_auth_identity` |
| 用户画像与偏好 | `user_profile` |
| 登录设备记录 | `user_device` |
| 邮箱验证码 | `email_verification` |
| 邮件/短信通知记录 | `notification_message` |

后续扩展：

- 邮箱注册和验证码校验。
- 手机号、QQ、微信、Google 登录入口。
- 密码找回和邮箱绑定。
- 登录设备列表与异常登录提醒。

### 8.2 学习概览

当前能力：

- 展示用户 ID、用户名、总积分、学习天数和当前排名。
- 提供刷题和排行榜快捷入口。
- 展示 Top 3 排名预览。

后端表映射：

| 前端能力 | 后端表 |
| --- | --- |
| 用户积分和学习天数 | `users.total_score`、`users.study_days` |
| 每日学习汇总 | `daily_study_summary` |
| 积分流水 | `score_transaction` |
| 排名数据 | `leaderboard_snapshot` 或实时聚合 |

后续扩展：

- 今日新学、复习、正确率、学习时长。
- 连续打卡天数、今日积分变化。
- 学习趋势图、技能雷达图、热力图。
- 当前词书和学习计划进度。

### 8.3 刷题练习

当前能力：

- 获取随机单词题。
- 展示单词和选项。
- 提交答案。
- 展示正确答案、积分变化和当前总积分。
- 答题后更新前端用户总积分。

后端表映射：

| 前端能力 | 后端表 |
| --- | --- |
| 单词基础信息 | `vocabulary_word` |
| 题目 | `question` |
| 选项 | `question_option` |
| 答题记录 | `question_attempt` |
| 单词学习事件 | `word_study_event` |
| 单词掌握状态 | `user_word_state` |
| 错题 | `wrong_answer` |
| 收藏 | `favorite_item` |

后续扩展：

- 词书选择和每日学习计划。
- 新学、复习、听音辨词、拼写题等题型。
- 错题自动归档。
- 收藏当前单词或题目。
- 记录 `duration_ms`、`client_event_id` 和答题来源。

### 8.4 排行榜

当前能力：

- 查看 Top 10、Top 20、Top 50。
- 表格展示排名、用户名和积分。
- ECharts 柱状图展示积分分布。

后端表映射：

| 前端能力 | 后端表 |
| --- | --- |
| 实时总积分 | `users.total_score` |
| 积分权威流水 | `score_transaction` |
| 排行榜快照 | `leaderboard_snapshot` |
| 学习小组 | `study_group`、`study_group_member` |
| 同桌/好友关系 | `study_partner` |

后续扩展：

- 全站榜、词书榜、小组榜、好友榜。
- 日榜、周榜、月榜、总榜。
- 我的排名、前后名次对比。
- 排名变化趋势。

### 8.5 阅读、听力与口语

当前 Web MVP 尚未实现，建议按以下模型扩展。

| 模块 | 页面能力 | 后端表 |
| --- | --- | --- |
| 阅读 | 文章列表、详情、阅读进度、阅读理解题 | `reading_passage`、`reading_progress`、`question`、`question_attempt` |
| 听力 | 听力材料列表、播放器、文稿、听力进度、听力题 | `listening_material`、`listening_progress`、`media_asset`、`question` |
| 口语 | 口语任务、录音、回放、评测结果 | `speaking_task`、`speaking_attempt`、`media_asset` |

内容来源遵循后端设计：

- 系统内容由管理后台统一录入。
- 用户上传内容可先保存元数据，正文或原始文件暂时保留在客户端。
- 后续需要云端保存时，通过 `content_storage_mode` 切换到 `OSS` 或 `SERVER`。

Web 端与 Qt 客户端不同，不适合承载大量本地原始内容；如需在 Web 端支持用户上传，建议直接走服务端或对象存储上传流程。

### 8.6 管理后台

管理后台是 Web 端后续重点扩展方向。

建议模块：

| 模块 | 页面能力 | 后端表 |
| --- | --- | --- |
| 用户管理 | 用户列表、状态管理、角色查看 | `users`、`user_profile`、`user_role` |
| 权限管理 | 角色、权限点、角色授权 | `role`、`permission`、`role_permission` |
| 内容管理 | 词书、单词、题库、阅读、听力、口语任务 | `book`、`vocabulary_word`、`question`、`reading_passage`、`listening_material`、`speaking_task` |
| 资源管理 | 图片、音频、视频、附件 | `media_asset` |
| 导入任务 | 批量导入词汇、题库、阅读材料 | `import_task` |
| 统计分析 | 用户活跃、正确率、内容使用数据 | `daily_study_summary`、`question_attempt`、`word_study_event` |
| 审计日志 | 管理员操作记录 | `audit_log` |
| 系统配置 | 配置项维护 | `system_config` |
| 标签知识点 | 标签、知识点、推荐预留 | `tag`、`knowledge_point`、`recommendation_log` |

普通学习端不展示角色/权限细节；只有管理员账号进入管理端后才展示管理入口。

## 9. 权限与安全

当前实现：

- 使用 JWT 作为登录凭证。
- 请求头使用 `Authorization: Bearer <token>`。
- 需要登录的路由配置 `meta.requiresAuth = true`。
- 401 响应触发本地登录态清理并跳转 `/auth`。

建议补充：

- 后端统一校验用户端和管理端权限。
- 管理端路由增加角色校验。
- 登录、注册、验证码接口增加限流。
- Token 过期时可引入 refresh token，当前 MVP 可先重新登录。
- 用户输入统一做前后端校验，后端以 Hibernate Validator 为准。
- 后端错误信息应区分用户可见信息和服务端排障信息。

## 10. 同步与离线策略

Qt 客户端采用“离线优先 + 最终同步”，需要 SQLite、本地事件队列和 `client_event_id` 幂等上传。

Web 端策略建议为“在线优先”：

- 不实现完整 SQLite 离线库。
- 学习事件提交以后端返回结果为准。
- 网络异常时只做页面级重试提示，不承诺离线学习可持续进行。
- 可使用浏览器本地存储保存草稿或短期 UI 状态，但不作为权威学习数据。
- 如果后续支持 PWA，可仅缓存静态资源和只读内容，学习事件仍应尽快提交后端。

仍需复用的同步字段和规则：

| 字段/规则 | Web 端使用方式 |
| --- | --- |
| `device_uuid` | 可由后端或前端生成，用于 Web 设备记录 |
| `client_event_id` | 重要提交可生成幂等 ID，避免重复提交 |
| `client_updated_at` | 用户侧操作时间 |
| `is_deleted` | 删除统一走逻辑删除 |
| `sync_task_log` | Web 端一般不直接展示，可在管理端用于排查 |

## 11. UI 与交互规范

当前 UI 采用 Element Plus 的后台型布局，适合学习端 MVP 和后续管理端。

建议规范：

- 页面使用清晰的标题、副标题和主操作按钮。
- 表单必须提供必填、长度、格式和错误提示。
- 所有异步请求提供 loading 状态。
- 空数据使用 `el-empty`。
- 操作成功和失败使用 `ElMessage` 反馈。
- 排行榜、统计分析等数据密集页面优先使用表格 + 图表组合。
- 移动端保留可用性，主要布局在窄屏下改为单列。
- 普通学习端避免展示底层权限、数据库字段和调试信息。

中文文案建议：

| 场景 | 文案方向 |
| --- | --- |
| 登录 | 简洁明确，突出账号、邮箱、验证码 |
| 学习反馈 | 即时、正向、指出下一步 |
| 错误提示 | 告诉用户发生了什么和能否重试 |
| 管理端 | 克制、信息密度高、便于批量操作 |

## 12. 数据模型映射总览

| Web 模块 | 主要后端表 |
| --- | --- |
| 账号登录 | `users`、`user_auth_identity`、`user_device`、`email_verification` |
| 用户资料 | `users`、`user_profile` |
| 学习概览 | `daily_study_summary`、`score_transaction`、`users` |
| 单词学习 | `book`、`book_word`、`vocabulary_word`、`user_word_plan`、`user_word_state`、`word_study_event` |
| 题库练习 | `question`、`question_option`、`question_attempt` |
| 错题收藏 | `wrong_answer`、`favorite_item` |
| 阅读 | `reading_passage`、`reading_progress` |
| 听力 | `listening_material`、`listening_progress`、`media_asset` |
| 口语 | `speaking_task`、`speaking_attempt`、`media_asset` |
| 打卡积分 | `checkin_record`、`score_transaction`、`achievement`、`user_achievement` |
| 排行榜 | `leaderboard_snapshot`、`study_group`、`study_group_member`、`study_partner` |
| 内容分享 | `content_share` |
| 管理后台 | `role`、`permission`、`audit_log`、`system_config`、`import_task` |
| 推荐预留 | `tag`、`knowledge_point`、`recommendation_log` |

## 13. MVP 优先级

### P0 已有或优先完成

- 登录、注册、退出登录。
- JWT Token 持久化和路由守卫。
- 学习概览。
- 在线刷题。
- 积分更新。
- 排行榜列表和图表。
- 后端统一响应解析。

### P1 建议跟进

- 邮箱注册、验证码、找回密码。
- 当前词书、学习计划、每日新学/复习目标。
- 单词学习事件记录与复习队列。
- 错题集和收藏夹。
- 今日学习汇总、打卡、积分流水。
- 排行榜范围扩展：周榜、月榜、词书榜、小组榜。
- Web 页面中文编码和文案统一校验。

### P2 后续扩展

- 阅读、听力、口语模块。
- 用户上传内容和对象存储。
- 语音评测 SDK 接入。
- 成就系统完整展示。
- 标签、知识点、推荐结果展示。
- 管理后台：内容录入、用户管理、权限、审计、导入任务。
- PWA 静态资源缓存和弱网体验优化。

## 14. 开发约定

接口封装：

- 新接口优先在 `src/api/modules.js` 中按领域追加模块。
- 页面组件不直接写 axios 配置。
- 需要复用的请求参数、分页参数和枚举值应抽成小函数或常量。

状态管理：

- 登录态、用户摘要放在 Pinia。
- 页面临时筛选条件优先放在组件内。
- 跨页面共享且会影响业务流程的状态再进入 store。

路由：

- 需要登录的页面统一放在 `MainLayout` 下。
- 管理端建议独立布局，例如 `AdminLayout`。
- 管理端路由增加角色或权限元信息。

错误处理：

- HTTP 层处理 Token、统一响应和 401。
- 页面层处理业务错误提示和空数据。
- 后端错误码稳定后，前端可根据 `code` 做更细的交互分支。

样式：

- 全局基础样式放在 `src/styles.css`。
- 页面私有样式使用 `<style scoped>`。
- 管理端页面保持表格、筛选栏、分页和批量操作一致。

## 15. 部署建议

生产构建产物位于：

```text
Web/dist/
```

推荐部署方式：

- 静态资源部署到 Nginx、OSS + CDN 或对象存储静态网站。
- API 请求由 Nginx 反向代理到 Spring Boot 后端。
- 前后端分离部署时，在构建环境设置 `VITE_API_BASE_URL`。
- 同源部署时，可使用 `/api/v1` 相对路径，减少跨域配置。

Nginx 方向示例：

```nginx
location / {
  try_files $uri $uri/ /index.html;
}

location /api/ {
  proxy_pass http://backend-service;
  proxy_set_header Host $host;
  proxy_set_header X-Real-IP $remote_addr;
}
```

生产注意事项：

- 开启 HTTPS。
- 配置静态资源缓存。
- 后端开启 CORS 或采用同源反向代理。
- 日志中避免输出明文 Token、密码和验证码。
- 管理端页面应与普通用户入口做权限隔离。

## 16. 当前实现与目标设计差距

| 方向 | 当前实现 | 目标设计 |
| --- | --- | --- |
| 账号 | 用户名/密码登录注册 | 邮箱、手机、第三方登录、设备记录 |
| 学习 | 随机刷题 | 词书、计划、复习算法、错题、收藏 |
| 统计 | 总积分、学习天数、排行榜 | 每日汇总、趋势、雷达图、热力图 |
| 内容 | 使用后端已有题目 | 词书、阅读、听力、口语、用户上传内容 |
| 管理 | 未实现 | 用户、内容、权限、审计、导入、配置 |
| 离线 | 不支持完整离线 | Web 在线优先，Qt 离线优先 |

当前 Web 端适合作为后端接口联调和在线学习 MVP。后续开发应优先补齐单词学习闭环，再扩展错题、统计和管理后台。
