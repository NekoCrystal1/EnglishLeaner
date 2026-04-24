# 客户端需求文档

> 根据 `Server/DataBaseDesign.md` v2 和 `Server/DatabaseER.mmd` 更新。  
> 适用范围：Qt 桌面客户端优先，移动端保持同一套信息架构和视觉规范；Web/管理端可复用服务端接口，但不在本文展开。

## 1. 客户端定位

- 同时支持桌面端和移动端，Qt 客户端作为主要实现。
- 客户端采用“离线优先、最终同步”的学习体验：核心学习内容、学习计划、答题记录、阅读/听力进度需要能在本地 SQLite 中保存，联网后再同步到服务端。
- 客户端需要覆盖数据库设计中的核心用户侧能力：账号登录、词书与单词计划、答题与错题、阅读理解、听力口语、学习统计、打卡积分、离线同步。
- 管理后台相关能力只在客户端预留展示入口或接口兼容，不作为普通学习客户端的核心页面。

## 2. 视觉与交互风格

- 桌面端和移动端使用统一视觉风格，尽量不直接使用系统原生 UI，优先自绘或使用统一 UI 组件库。
- 视觉方向可采用像素风，参考 `Client_QT/示例ui风格.png`，但需要保证阅读、答题和长文本场景的清晰度。
- 支持跟随系统浅色/深色模式切换主题色。
- 登录页、首页可根据系统主题切换背景图或背景色。
- 常用操作需要有明确反馈：加载中、离线、同步中、同步失败、答题正确/错误、收藏成功、计划保存成功。

## 3. 账号、登录与用户资料

对应服务端表：`users`、`user_auth_identity`、`user_profile`、`user_device`、`email_verification`、`notification_message`。

### 登录页面

- 展示应用名称，名称暂可自拟或留空。
- 展示一张根据系统颜色模式切换的背景图。
- 支持账号/邮箱登录作为首期主流程。
- 预留手机号、微信、QQ 登录入口，对应 `user_auth_identity.provider`。
- 邮箱注册或绑定时需要支持验证码发送、验证码校验、验证码过期提示。
- 登录成功后记录当前设备信息，对应 `user_device.device_uuid`、`platform`、`app_version`、`last_seen_at`。

### 用户资料与设置

- 展示头像、昵称、邮箱、手机号、英语水平、目标水平、每日学习时长。
- 支持修改学习偏好，对应 `user_profile.english_level`、`target_level`、`daily_minutes`、`timezone`、`preference_json`。
- 展示当前设备和同步状态。
- 支持退出登录；退出时本地数据需要按用户隔离，避免不同账号学习记录混用。

## 4. 应用主框架与导航

底部导航栏建议包含：

- 单词：首页和单词学习主入口。
- 听说：听力材料、口语任务和练习记录。
- 阅读：阅读文章、阅读进度和阅读理解题。
- 我的：学习统计、打卡、积分、排行榜、成就、账号设置和同步状态。

各学习板块右上角保留“学习状态”入口，用于查看当前板块的进度、历史记录、错题、收藏和同步状态。

## 5. 首页与单词板块

对应服务端表：`book`、`book_word`、`vocabulary_word`、`user_word_plan`、`user_word_state`、`word_study_event`、`daily_study_summary`、`score_transaction`。

### 首页

- 首页即单词板块首页。
- 每日更新一张背景图。
- 每日生成一个励志单词或一句学习提示。
- 展示今日学习概览：新学单词数、复习单词数、正确率、学习时长、连续学习天数、今日积分变化。
- 展示当前学习词书，点击后进入词书和计划设置页面。
- 底部展示当前进度、今日新学目标、今日复习目标。
- 点击“开始学习”后进入单词学习页面。

### 词书与计划设置

- 展示可用词书列表，包括系统内置词书和用户本地上传词书。
- 词书元数据对应 `book`，词书单词关系对应 `book_word`，单词基础信息对应 `vocabulary_word`。
- 支持选择当前学习词书，并创建或切换 `user_word_plan`。
- 支持设置每日新学数量、每日复习数量、复习算法类型。
- 展示当前词书进度：总词数、已学、待复习、已掌握。
- 用户上传书籍时，服务端默认只保存 `book` 元数据、`client_content_ref`、`content_hash`；正文保存在客户端本地。

### 单词学习页面

- 展示单词、英/美音标、发音按钮、释义、词性、例句、难度。
- 支持选择题、认识/不认识、拼写或听音辨词等题型扩展。
- 选择题选项优先来自 `question_option`；单词题也可以由 `vocabulary_word` 临时生成。
- 每次学习、复习、答题都生成一条 `word_study_event`，离线时先写入本地待同步队列。
- 答题后即时更新本地 `user_word_state`：正确次数、错误次数、掌握度、下次复习时间。
- 支持收藏当前单词或题目，对应 `favorite_item`。
- 答错题目需要进入错题集，对应 `wrong_answer`。

### 单词状态页

- 展示当前计划的学习状态：学习中、待复习、已掌握、易错词。
- 展示复习队列和最近学习事件。
- 展示日历或趋势图，数据来自 `daily_study_summary`。

## 6. 题库、错题与收藏

对应服务端表：`question`、`question_option`、`question_attempt`、`wrong_answer`、`favorite_item`。

- 所有模块的答题记录统一写入 `question_attempt`。
- 题目需要支持单词题、阅读理解题、听力题等模块类型，对应 `question.module_type`。
- 答题后展示正确答案、解析、得分变化和耗时。
- 错题集按模块筛选：单词、阅读、听力、口语。
- 错题复习答对后可标记为已解决，对应 `wrong_answer.status = RESOLVED`。
- 收藏夹支持按类型展示：单词、题目、阅读文章、听力材料。
- 收藏夹预留文件夹名称，对应 `favorite_item.folder_name`。

## 7. 阅读理解板块

对应服务端表：`reading_passage`、`reading_progress`、`question`、`question_attempt`。

- 展示阅读文章列表，支持按难度、词数、预计阅读时长筛选。
- 阅读详情页展示标题、正文、摘要、难度、预计阅读时间。
- 需要记录阅读进度：百分比、最后阅读位置、阅读秒数、完成时间。
- 阅读理解题通过 `question.source_type = READING_PASSAGE`、`question.source_id = reading_passage.id` 关联。
- 阅读页支持收藏文章、继续阅读、完成后答题。
- 离线阅读进度需要写入本地，联网后同步。

## 8. 听力与口语板块

对应服务端表：`listening_material`、`listening_progress`、`speaking_task`、`speaking_attempt`、`media_asset`。

### 听力

- 展示听力材料列表，支持按难度、时长筛选。
- 听力详情页包含标题、音频播放器、文本稿、学习进度。
- 记录听力进度：已听秒数、完成次数、最后播放位置。
- 听力题可复用统一题库和答题记录。

### 口语

- 展示口语任务列表，包含提示文本、参考文本、难度。
- 支持录音、回放、提交评测。
- 首期可以先保存录音和作答记录；第三方语音评测 SDK 接入后，将识别文本、总分、细项分数写入 `speaking_attempt`。
- 音频资源对应 `media_asset`，是否上传服务端对象存储待后续确认。

## 9. 学习统计、打卡与激励

对应服务端表：`daily_study_summary`、`checkin_record`、`score_transaction`、`achievement`、`user_achievement`、`leaderboard_snapshot`、`users.total_score`。

- 我的页面展示今日学习汇总、连续学习天数、总积分、已获得成就。
- 支持每日打卡，打卡后展示连续天数和奖励积分。
- 展示积分流水，说明积分来源：答题、复习、打卡、成就奖励。
- 排行榜首期可按 `users.total_score` 展示全站总榜。
- 后续可按周榜、月榜、词书榜或班级榜扩展，数据可来自 `leaderboard_snapshot`。
- 成就系统首期可只展示已获得和未获得状态，复杂条件由服务端计算。

## 10. 离线数据与同步

对应服务端表：`user_device`、`sync_task_log`、`sync_change_item`、`server_change_log`，对应客户端本地表：`local_sync_outbox`、`local_sync_meta`。

- 客户端首次登录后生成并保存 `device_uuid`。
- 所有离线创建的学习事件都需要生成稳定的 `client_event_id`，重试上传时保持不变。
- 本地数据同时保存 `local_id` 和 `server_id`：`local_id` 用于离线立即可用，`server_id` 用于同步成功后的服务端引用。
- 客户端上传同步时携带 `device_uuid`、`client_event_id`、`client_updated_at`。
- 服务端以 `user_id + client_event_id` 做幂等去重。
- 普通学习记录冲突采用“最后修改时间优先”；账号、安全、权限类数据采用“服务端优先”。
- 删除操作统一按逻辑删除处理，并同步 `is_deleted = 1`。
- UI 需要展示同步状态：本地未同步、同步中、已同步、同步失败、需要重试。
- 网络恢复后自动尝试上传 `local_sync_outbox` 中的待同步变更，并拉取 `last_pulled_version` 之后的服务端增量。

## 11. 本地 SQLite 子集

Qt 客户端需要保存以下本地表或等价结构：

| 客户端数据 | 服务端对应表 | 本地建议表 |
| --- | --- | --- |
| 当前用户和偏好 | `users`、`user_profile`、`user_device` | `local_user_profile`、`local_sync_meta` |
| 词书元数据 | `book` | `local_book` |
| 用户上传书籍正文 | `book.client_content_ref` | `local_book_content` |
| 单词缓存 | `vocabulary_word`、`book_word` | `local_vocabulary_word`、`local_book_word` |
| 单词计划和复习状态 | `user_word_plan`、`user_word_state` | `local_user_word_plan`、`local_user_word_state` |
| 单词学习事件 | `word_study_event` | `local_word_study_event`、`local_sync_outbox` |
| 统一答题记录 | `question_attempt` | `local_question_attempt`、`local_sync_outbox` |
| 阅读进度 | `reading_progress` | 可并入本地进度表或单独建表 |
| 听力进度 | `listening_progress` | 可并入本地进度表或单独建表 |
| 同步元数据 | `sync_task_log`、`server_change_log` | `local_sync_meta`、`local_sync_outbox` |

## 12. MVP 优先级

### P0 必做

- 账号注册、邮箱/账号登录、登录态保存。
- 首页、词书选择、单词计划设置。
- 单词学习、复习、答题、学习事件记录。
- 本地 SQLite 保存当前用户、词书、单词计划、学习事件。
- 基础同步：离线事件上传、同步状态展示、失败重试。
- 我的页面展示今日学习汇总、积分、连续学习天数。

### P1 建议跟进

- 错题集、收藏夹。
- 阅读文章列表、阅读进度、阅读理解题。
- 听力材料列表、播放器、听力进度。
- 每日打卡、积分流水、排行榜。
- 用户上传词书/书籍的本地解析与元数据登记。

### P2 后续扩展

- QQ、微信、手机号登录绑定。
- 口语录音、语音评测 SDK、音频对象存储。
- 成就系统完整展示。
- 标签、知识点、推荐结果展示。
- 管理后台内容录入和权限细分。

## 13. 待确认问题

- 用户上传书籍正文是否长期只保存在客户端，还是后续允许同步到对象存储。
- 阅读、听力、口语内容是否由管理后台统一录入，还是允许客户端导入本地材料。
- 排行榜范围是否只有全站总榜，还是需要词书榜、班级榜、周榜/月榜。
- 普通客户端是否需要展示角色/权限信息，还是仅管理端使用。
- 语音评测接入后，用户录音是否必须上传服务端。
