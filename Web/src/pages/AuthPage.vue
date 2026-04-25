<template>
  <div class="auth-page">
    <section class="brand-panel">
      <div class="brand-badge">English Learner</div>
      <h1>把单词、测验和学习进度放在一起</h1>
      <p>登录后可以学习词书、记录复习情况，并通过测验获得积分。</p>
    </section>

    <section class="form-panel">
      <el-tabs v-model="activeTab" stretch>
        <el-tab-pane label="登录" name="login">
          <el-form ref="loginFormRef" :model="loginForm" :rules="loginRules" label-position="top">
            <el-form-item prop="username" label="用户名">
              <el-input
                v-model.trim="loginForm.username"
                placeholder="请输入用户名"
                clearable
                autocomplete="username"
                @keyup.enter="submitLogin"
              />
            </el-form-item>
            <el-form-item prop="password" label="密码">
              <el-input
                v-model="loginForm.password"
                placeholder="请输入密码"
                type="password"
                show-password
                autocomplete="current-password"
                @keyup.enter="submitLogin"
              />
            </el-form-item>
            <el-button :loading="authStore.loading" type="primary" class="action-btn" @click="submitLogin">
              登录
            </el-button>
          </el-form>
        </el-tab-pane>

        <el-tab-pane label="注册" name="register">
          <el-form ref="registerFormRef" :model="registerForm" :rules="registerRules" label-position="top">
            <el-form-item prop="username" label="用户名">
              <el-input
                v-model.trim="registerForm.username"
                placeholder="2-32 个字符"
                clearable
                autocomplete="username"
              />
            </el-form-item>
            <el-form-item prop="password" label="密码">
              <el-input
                v-model="registerForm.password"
                type="password"
                show-password
                placeholder="6-64 个字符"
                autocomplete="new-password"
              />
            </el-form-item>
            <el-form-item prop="confirmPassword" label="确认密码">
              <el-input
                v-model="registerForm.confirmPassword"
                type="password"
                show-password
                autocomplete="new-password"
                @keyup.enter="submitRegister"
              />
            </el-form-item>
            <el-button :loading="authStore.loading" type="success" class="action-btn" @click="submitRegister">
              注册并登录
            </el-button>
          </el-form>
        </el-tab-pane>
      </el-tabs>
    </section>
  </div>
</template>

<script setup>
import { computed, reactive, ref } from "vue";
import { ElMessage } from "element-plus";
import { useRoute, useRouter } from "vue-router";
import { useAuthStore } from "../stores/auth";

const route = useRoute();
const router = useRouter();
const authStore = useAuthStore();

const activeTab = ref("login");
const loginFormRef = ref();
const registerFormRef = ref();

const loginForm = reactive({
  username: "",
  password: ""
});

const registerForm = reactive({
  username: "",
  password: "",
  confirmPassword: ""
});

const redirectPath = computed(() => {
  const fromQuery = route.query.redirect;
  if (!fromQuery || typeof fromQuery !== "string" || fromQuery === "/auth") {
    return "/";
  }
  return fromQuery;
});

const loginRules = {
  username: [{ required: true, message: "请输入用户名", trigger: "blur" }],
  password: [{ required: true, message: "请输入密码", trigger: "blur" }]
};

const registerRules = {
  username: [
    { required: true, message: "请输入用户名", trigger: "blur" },
    { min: 2, max: 32, message: "用户名长度为 2-32 个字符", trigger: "blur" }
  ],
  password: [
    { required: true, message: "请输入密码", trigger: "blur" },
    { min: 6, max: 64, message: "密码长度为 6-64 个字符", trigger: "blur" }
  ],
  confirmPassword: [
    {
      validator: (_, value, callback) => {
        if (!value) {
          callback(new Error("请再次输入密码"));
          return;
        }
        if (value !== registerForm.password) {
          callback(new Error("两次输入的密码不一致"));
          return;
        }
        callback();
      },
      trigger: "blur"
    }
  ]
};

async function submitLogin() {
  if (!loginFormRef.value) {
    return;
  }

  try {
    await loginFormRef.value.validate();
    await authStore.login({
      username: loginForm.username,
      password: loginForm.password
    });
    ElMessage.success("登录成功");
    router.replace(redirectPath.value);
  } catch (error) {
    if (error?.message) {
      ElMessage.error(error.message);
    }
  }
}

async function submitRegister() {
  if (!registerFormRef.value) {
    return;
  }

  try {
    await registerFormRef.value.validate();
    await authStore.registerAndLogin({
      username: registerForm.username,
      password: registerForm.password
    });
    ElMessage.success("注册成功，已自动登录");
    router.replace("/");
  } catch (error) {
    if (error?.message) {
      ElMessage.error(error.message);
    }
  }
}
</script>

<style scoped>
.auth-page {
  min-height: 100vh;
  display: grid;
  grid-template-columns: minmax(0, 1.05fr) minmax(360px, 0.95fr);
  align-items: center;
  gap: 28px;
  width: min(1080px, 100%);
  margin: 0 auto;
  padding: 24px;
}

.brand-panel {
  min-height: 420px;
  display: flex;
  flex-direction: column;
  justify-content: center;
  padding: 38px;
  border-radius: 12px;
  color: #ffffff;
  background:
    linear-gradient(rgba(15, 23, 42, 0.32), rgba(15, 23, 42, 0.46)),
    url("https://images.unsplash.com/photo-1516321318423-f06f85e504b3?auto=format&fit=crop&w=1200&q=80") center/cover;
  overflow: hidden;
}

.brand-badge {
  width: fit-content;
  margin-bottom: 18px;
  padding: 8px 12px;
  border-radius: 8px;
  background: rgba(255, 255, 255, 0.18);
  font-weight: 700;
}

.brand-panel h1 {
  max-width: 560px;
  margin: 0 0 14px;
  font-size: 40px;
  line-height: 1.18;
}

.brand-panel p {
  max-width: 480px;
  margin: 0;
  color: rgba(255, 255, 255, 0.88);
  font-size: 17px;
}

.form-panel {
  padding: 24px;
  border-radius: 12px;
  background: rgba(255, 255, 255, 0.96);
  border: 1px solid #e5e7eb;
  box-shadow: 0 20px 50px rgba(15, 23, 42, 0.08);
}

.action-btn {
  width: 100%;
}

@media (max-width: 900px) {
  .auth-page {
    grid-template-columns: 1fr;
    padding: 14px;
  }

  .brand-panel {
    min-height: 260px;
    padding: 28px;
  }

  .brand-panel h1 {
    font-size: 30px;
  }
}
</style>
