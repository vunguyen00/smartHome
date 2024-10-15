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

// Đặt các hàm trong phạm vi toàn cục để HTML có thể gọi chúng
window.setLEDStatus = setLEDStatus;
window.getLEDStatus = getLEDStatus;
window.turnOffAllLEDs = turnOffAllLEDs; // Đặt hàm vào phạm vi toàn cục

// Lấy trạng thái LED khi tải trang
window.onload = () => {
  getLEDStatus('led1');
  getLEDStatus('led2');
  getLEDStatus('led3');
};
