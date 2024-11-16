// Import Firebase SDK
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.6.8/firebase-app.js";
import { getDatabase, ref, set, get } from "https://www.gstatic.com/firebasejs/9.6.8/firebase-database.js";
import { getAuth, signInAnonymously } from "https://www.gstatic.com/firebasejs/9.6.8/firebase-auth.js";

// Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyCg6TJ2KdWKWI5QLvst8oBscm8Em065Vm4",
  authDomain: "esp8266-9c680.firebaseapp.com",
  databaseURL: "https://esp8266-9c680-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "esp8266-9c680",
  storageBucket: "esp8266-9c680.appspot.com",
  messagingSenderId: "559844471454",
  appId: "1:559844471454:web:811c87bd96e7ad29df418f",
  measurementId: "G-VKTQ727H7R"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);
const auth = getAuth(app);

// Anonymous login
signInAnonymously(auth)
  .then(() => {
    console.log('Đăng nhập vô danh thành công!');
    checkFirebaseConnection();
  })
  .catch((error) => {
    console.error('Lỗi đăng nhập vô danh: ', error);
  });

// Kiểm tra kết nối Firebase
function checkFirebaseConnection() {
  get(ref(database, '/')).then((snapshot) => {
    if (snapshot.exists()) {
      console.log("Kết nối Firebase thành công!");
      document.getElementById('connectionStatus').innerText = "Kết nối thành công!";
    } else {
      console.log("Không có dữ liệu.");
    }
  }).catch((error) => {
    console.error("Kết nối Firebase thất bại: ", error);
    document.getElementById('connectionStatus').innerText = "Kết nối thất bại!";
  });
}

// Thiết lập trạng thái cho từng LED
function setLEDStatus(led, status) {
  set(ref(database, `led/${led}/status`), status)
    .then(() => {
      console.log(`${led} status updated to: ${status}`);
      document.getElementById(`${led}Status`).innerText = `${led} is ${status}`; // Cập nhật trạng thái ngay
      getLEDStatus(led); // Cập nhật lại trạng thái
    })
    .catch((error) => {
      console.error(`Error updating ${led} status: `, error);
    });
}

// Lấy trạng thái LED hiện tại
function getLEDStatus(led) {
  get(ref(database, `led/${led}/status`)).then((snapshot) => {
    if (snapshot.exists()) {
      const status = snapshot.val();
      console.log(`Current ${led} status: ${status}`);
      document.getElementById(`${led}Status`).innerText = `Current ${led} status: ${status}`;
    } else {
      console.log("No data available");
    }
  }).catch((error) => {
    console.error(`Error getting ${led} status: `, error);
  });
}

// Hàm để tắt tất cả các LED
function turnOffAllLEDs() {
  const ledRefs = ['led1', 'led2', 'led3']; // Danh sách LED cần tắt
  ledRefs.forEach(led => {
    setLEDStatus(led, 'off'); // Tắt từng LED
  });
  console.log("All LEDs have been turned off.");
}
function turnOnAllLEDs() {
  const ledRefs = ['led1', 'led2', 'led3']; // Danh sách LED cần tắt
  ledRefs.forEach(led => {
    setLEDStatus(led, 'on'); // Tắt từng LED
  });
  console.log("All LEDs have been turned off.");
}
// Hàm để thiết lập trạng thái cửa (mở hoặc đóng)
function setDoorStatus(status) {
  set(ref(database, `door/status`), status)
    .then(() => {
      console.log(`Door status updated to: ${status}`);
      document.getElementById('doorStatus').innerText = `Door is ${status}`; // Cập nhật trạng thái ngay trên giao diện
      getDoorStatus(); // Lấy lại trạng thái cửa
    })
    .catch((error) => {
      console.error(`Error updating door status: `, error);
    });
}

// Hàm để lấy trạng thái hiện tại của cửa
function getDoorStatus() {
  get(ref(database, `door/status`)).then((snapshot) => {
    if (snapshot.exists()) {
      const status = snapshot.val();
      console.log(`Current door status: ${status}`);
      document.getElementById('doorStatus').innerText = `Current door status: ${status}`;
    } else {
      console.log("No door status data available");
    }
  }).catch((error) => {
    console.error(`Error getting door status: `, error);
  });
}
// Cập nhật trạng thái của tất cả LED
function updateLEDStatus(status) {
  const ledRefs = ['led1', 'led2', 'led3'];
  ledRefs.forEach(led => {
    set(ref(database, `led/${led}/status`), status)
      .then(() => console.log(`${led} status updated to: ${status}`))
      .catch((error) => console.error(`Error updating ${led} status: `, error));
  });
}

// Đặt thời gian cho LED và lưu vào Firebase
function setTimerForAll() {
  const onTime = document.getElementById("ledOnTime").value;
  const offTime = document.getElementById("ledOffTime").value;

  if (onTime && offTime) {
      set(ref(database, "led/onTime"), onTime);
      set(ref(database, "led/offTime"), offTime);
      alert(`Timer set for all LEDs: ON at ${onTime}, OFF at ${offTime}`);
  } else {
      alert("Please enter both On and Off times.");
  }
}

// Kiểm tra và cập nhật trạng thái của từng LED dựa trên thời gian
function checkLEDTimer() {
  get(ref(database, "led")).then((snapshot) => {
    if (snapshot.exists()) {
      const ledData = snapshot.val();
      const currentTime = new Date();
      const currentHours = String(currentTime.getHours()).padStart(2, '0');
      const currentMinutes = String(currentTime.getMinutes()).padStart(2, '0');
      const formattedTime = `${currentHours}:${currentMinutes}`;

      const ledRefs = ['led1', 'led2', 'led3']; // Danh sách các LED cần cập nhật

      if (formattedTime === ledData.onTime) {
        // Bật từng LED khi đến giờ onTime
        ledRefs.forEach(led => {
          setLEDStatus(led, 'on'); // Cập nhật trạng thái từng LED thành "on"
        });
        console.log("All LEDs have been turned on.");
      } else if (formattedTime === ledData.offTime) {
        // Tắt từng LED khi đến giờ offTime
        ledRefs.forEach(led => {
          setLEDStatus(led, 'off'); // Cập nhật trạng thái từng LED thành "off"
        });
        console.log("All LEDs have been turned off.");
      }
    }
  }).catch((error) => console.error("Error checking LED timer: ", error));
}

// Thiết lập để kiểm tra giờ onTime và offTime mỗi phút
setInterval(checkLEDTimer, 15000); // Kiểm tra mỗi phút

// Hiển thị/ẩn phần nhập liệu đổi mật khẩu
function toggleChangePasswordSection() {
  const changePasswordContainer = document.getElementById("changePasswordContainer");
  changePasswordContainer.style.display = changePasswordContainer.style.display === "none" ? "block" : "none";
}

// Hàm đổi mật khẩu và lưu vào Firebase
function changePassword() {
  const currentPassword = document.getElementById("currentPassword").value;
  const newPassword = document.getElementById("newPassword").value;
  const confirmPassword = document.getElementById("confirmPassword").value;
  const statusMessage = document.getElementById("passwordChangeStatus");

  // Kiểm tra nếu mật khẩu mới và xác nhận khớp nhau
  if (newPassword !== confirmPassword) {
      statusMessage.textContent = "Passwords do not match.";
      statusMessage.style.color = "red";
      return;
  }

  // Ghi mật khẩu mới vào Firebase Realtime Database
  const passwordRef = ref(database, 'door/password');
  get(passwordRef)
      .then((snapshot) => {
          if (snapshot.exists() && snapshot.val() === currentPassword) {
              set(passwordRef, newPassword)
                  .then(() => {
                      statusMessage.textContent = "Password changed successfully!";
                      statusMessage.style.color = "green";
                  })
                  .catch((error) => {
                      console.error("Error updating password: ", error);
                      statusMessage.textContent = "Failed to update password.";
                      statusMessage.style.color = "red";
                  });
          } else {
              statusMessage.textContent = "Current password is incorrect.";
              statusMessage.style.color = "red";
          }
      })
      .catch((error) => {
          console.error("Error reading current password: ", error);
          statusMessage.textContent = "Error reading current password.";
          statusMessage.style.color = "red";
      });
}
// Đặt thời gian cho PIR và lưu vào Firebase
function setPIRTimer() {
  const onTime = document.getElementById("pirOnTime").value;
  const offTime = document.getElementById("pirOffTime").value;

  if (onTime && offTime) {
      // Lưu thời gian vào Firebase
      set(ref(database, "pir/onTime"), onTime);
      set(ref(database, "pir/offTime"), offTime);
      alert(`PIR Timer set: ON at ${onTime}, OFF at ${offTime}`);
  } else {
      alert("Please enter both On and Off times.");
  }
}
// Hàm để cập nhật trạng thái PIR khi có sự thay đổi
function updatePIRStatus(status) {
  set(ref(database, 'pir/status'), status)
    .then(() => {
      console.log(`PIR status updated to: ${status}`);
      document.getElementById('pirStatus').innerText = `PIR Status: ${status}`; // Update the UI immediately
    })
    .catch((error) => {
      console.error(`Error updating PIR status: `, error);
    });
}

// Kiểm tra và cập nhật trạng thái của PIR dựa trên thời gian
function checkPIRTimer() {
  get(ref(database, "pir")).then((snapshot) => {
    if (snapshot.exists()) {
      const pirData = snapshot.val();
      const currentTime = new Date();
      const currentHours = String(currentTime.getHours()).padStart(2, '0');
      const currentMinutes = String(currentTime.getMinutes()).padStart(2, '0');
      const formattedTime = `${currentHours}:${currentMinutes}`;

      // Kiểm tra nếu thời gian hiện tại trùng với thời gian ON hoặc OFF
      if (formattedTime === pirData.onTime) {
        // Bật PIR khi đến giờ onTime
        updatePIRStatus("active"); 
        console.log("PIR sensor activated.");
      } else if (formattedTime === pirData.offTime) {
        // Tắt PIR khi đến giờ offTime
        updatePIRStatus("inactive"); 
        console.log("PIR sensor deactivated.");
      }
    }
  }).catch((error) => console.error("Error checking PIR timer: ", error));
}

setInterval(checkPIRTimer, 30000); // Kiểm tra mỗi 30 giây

// Đặt các hàm trong phạm vi toàn cục để HTML có thể gọi chúng
window.setLEDStatus = setLEDStatus;
window.getLEDStatus = getLEDStatus;
window.turnOffAllLEDs = turnOffAllLEDs; // Đặt hàm vào phạm vi toàn cục
window.setDoorStatus = setDoorStatus;
window.getDoorStatus = getDoorStatus;
window.setTimerForAll = setTimerForAll;
window.toggleChangePasswordSection = toggleChangePasswordSection;
window.changePassword = changePassword;
window.turnOnAllLEDs = turnOnAllLEDs;
window.setPIRTimer = setPIRTimer;
window.updatePIRStatus = updatePIRStatus;


// Lấy trạng thái LED khi tải trang
window.onload = () => {
  getLEDStatus('led1');
  getLEDStatus('led2');
  getLEDStatus('led3');
  getDoorStatus();
};