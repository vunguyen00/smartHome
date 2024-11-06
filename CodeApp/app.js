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

// Thiết lập trạng thái cho từng LED với chế độ ưu tiên điều khiển thủ công
function setLEDStatus(led, status) {
  set(ref(database, `led/${led}/status`), status)
    .then(() => {
      console.log(`${led} status updated to: ${status}`);
      document.getElementById(`${led}Status`).innerText = `${led} is ${status}`;
      set(ref(database, `led/${led}/manualControl`), true);
    })
    .catch((error) => {
      console.error(`Error updating ${led} status: `, error);
    });
}

// Cập nhật thời gian bật/tắt cho từng LED
function setLEDSchedule(led, onTime, offTime) {
  set(ref(database, `led/${led}/schedule`), { onTime, offTime })
    .then(() => {
      console.log(`Scheduled ${led} onTime: ${onTime}, offTime: ${offTime}`);
    })
    .catch((error) => {
      console.error(`Error scheduling ${led}: `, error);
    });
}

// Kiểm tra và thực hiện lịch trình cho từng LED
function checkLEDSchedule() {
  const ledRefs = ['led1', 'led2', 'led3'];
  const currentTime = new Date().toLocaleTimeString("en-US", { hour12: false });

  ledRefs.forEach(led => {
    get(ref(database, `led/${led}/schedule`)).then((snapshot) => {
      if (snapshot.exists()) {
        const { onTime, offTime } = snapshot.val();
        const currentStatus = document.getElementById(`${led}Status`).innerText.includes("on");

        if (currentTime >= onTime && currentTime < offTime && !currentStatus) {
          setLEDStatus(led, 'on');
        } else if (currentTime >= offTime && currentStatus) {
          setLEDStatus(led, 'off');
        }
      }
    }).catch((error) => {
      console.error(`Error getting schedule for ${led}: `, error);
    });
  });
}

// Hàm để tắt tất cả các LED
function turnOffAllLEDs() {
  const ledRefs = ['led1', 'led2', 'led3'];
  ledRefs.forEach(led => {
    setLEDStatus(led, 'off');
  });
  console.log("All LEDs have been turned off.");
}

// Vòng lặp kiểm tra hẹn giờ và trạng thái
setInterval(() => {
  checkLEDSchedule();
  const ledRefs = ['led1', 'led2', 'led3'];
  ledRefs.forEach(led => {
    get(ref(database, `led/${led}/manualControl`)).then((snapshot) => {
      if (snapshot.exists() && snapshot.val()) {
        set(ref(database, `led/${led}/manualControl`), false);
      }
    });
  });
}, 60000); // Kiểm tra mỗi phút

window.setLEDStatus = setLEDStatus;
window.setLEDSchedule = setLEDSchedule;
window.turnOffAllLEDs = turnOffAllLEDs;

window.onload = () => {
  getLEDStatus('led1');
  getLEDStatus('led2');
  getLEDStatus('led3');
};


function setDoorStatus(status) {
  set(ref(database, 'door/status'), status)
    .then(() => {
      console.log(`Door status updated to: ${status}`);
      document.getElementById('doorStatus').innerText = `Door is ${status}`; // Cập nhật trạng thái ngay
      logDoorActivity(status); // Ghi lại hoạt động của cửa
      getDoorStatus(); // Cập nhật lại trạng thái
    })
    .catch((error) => {
      console.error(`Error updating door status: `, error);
    });
}
// Ghi lại hoạt động của cửa với thời gian
function logDoorActivity(action) {
  const timestamp = new Date().toISOString(); // Lấy thời gian hiện tại
  set(ref(database, `door/log/${timestamp}`), { action: action, timestamp: timestamp })
    .then(() => {
      console.log(`Door activity logged: ${action} at ${timestamp}`);
    })
    .catch((error) => {
      console.error("Error logging door activity: ", error);
    });
}

function getDoorStatus() {
  get(ref(database, 'door/status')).then((snapshot) => {
    if (snapshot.exists()) {
      const status = snapshot.val();
      console.log(`Current door status: ${status}`);
      document.getElementById('doorStatus').innerText = `Current door status: ${status}`;
    } else {
      console.log("No data available for door.");
    }
  }).catch((error) => {
    console.error("Error getting door status: ", error);
  });
}

window.setDoorStatus = setDoorStatus;
window.getDoorStatus = getDoorStatus;