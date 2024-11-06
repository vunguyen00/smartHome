// Nhập các thư viện Firebase để cấu hình, truy cập cơ sở dữ liệu, và xác thực
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.6.8/firebase-app.js";
import { getDatabase, ref, set, get } from "https://www.gstatic.com/firebasejs/9.6.8/firebase-database.js";
import { getAuth, signInAnonymously } from "https://www.gstatic.com/firebasejs/9.6.8/firebase-auth.js";

// Cấu hình Firebase
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

const app = initializeApp(firebaseConfig);
const database = getDatabase(app); 
const auth = getAuth(app);

signInAnonymously(auth)
  .then(() => {
    console.log('Đăng nhập vô danh thành công!');
    checkFirebaseConnection(); 
  })
  .catch((error) => {
    console.error('Lỗi đăng nhập vô danh: ', error);
  });

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

// Thiết lập trạng thái cho từng LED trong cơ sở dữ liệu
function setLEDStatus(led, status) {
  set(ref(database, `led/${led}/status`), status) // Lưu trạng thái mới (bật/tắt) của LED vào Firebase
    .then(() => {
      console.log(`${led} status updated to: ${status}`);
      document.getElementById(`${led}Status`).innerText = `${led} is ${status}`; // Cập nhật trạng thái trên giao diện người dùng
    })
    .catch((error) => {
      console.error(`Error updating ${led} status: `, error);
    });
}

// Thiết lập thời gian bật/tắt theo lịch cho từng LED trong cơ sở dữ liệu
function setLEDSchedule(led, onTime, offTime) {
  set(ref(database, `led/${led}/schedule`), { onTime, offTime }) // Lưu thời gian bật/tắt của LED vào Firebase
    .then(() => {
      console.log(`Scheduled ${led} onTime: ${onTime}, offTime: ${offTime}`);
    })
    .catch((error) => {
      console.error(`Error scheduling ${led}: `, error);
    });
}

// Kiểm tra lịch trình bật/tắt cho từng LED dựa trên thời gian hiện tại
function checkLEDSchedule() {
  const ledRefs = ['led1', 'led2', 'led3']; // Mảng chứa tên các đèn LED để kiểm tra tuần tự
  const currentTime = new Date().toLocaleTimeString("en-US", { hour12: false }); // Thời gian hiện tại ở định dạng HH:MM:SS

  ledRefs.forEach(led => {
    get(ref(database, `led/${led}/schedule`)).then((snapshot) => { // Lấy lịch trình bật/tắt của LED từ Firebase
      if (snapshot.exists()) {
        const { onTime, offTime } = snapshot.val(); // Thời gian bật và tắt theo lịch
        get(ref(database, `led/${led}/manualControl`)).then((manualSnapshot) => { // Kiểm tra nếu LED có đang ở chế độ điều khiển thủ công không
          const isManualControl = manualSnapshot.exists() && manualSnapshot.val();
          if (!isManualControl) { // Nếu không có điều khiển thủ công, đèn sẽ tuân theo lịch trình bật/tắt
            if (currentTime >= onTime && currentTime < offTime) {
              setLEDStatus(led, 'on'); // Bật đèn nếu đang trong khoảng thời gian bật
            } else {
              setLEDStatus(led, 'off'); // Tắt đèn nếu không nằm trong thời gian bật
            }
          }
        });
      }
    }).catch((error) => {
      console.error(`Error getting schedule for ${led}: `, error);
    });
  });
}

// Tắt tất cả các LED và cập nhật Firebase
function turnOffAllLEDs() {
  const ledRefs = ['led1', 'led2', 'led3'];
  ledRefs.forEach(led => {
    setLEDStatus(led, 'off'); // Tắt từng LED
  });
  console.log("All LEDs have been turned off.");
}

// Tự động kiểm tra và thực hiện lịch trình bật/tắt mỗi 60 giây
setInterval(checkLEDSchedule, 60000);

// Xuất các hàm ra phạm vi toàn cục để có thể truy cập từ HTML
window.setLEDStatus = setLEDStatus;
window.setLEDSchedule = setLEDSchedule;
window.turnOffAllLEDs = turnOffAllLEDs;

function setDoorStatus(status) {
  set(ref(database, 'door/status'), status) 
    .then(() => {
      console.log(`Door status updated to: ${status}`);
      document.getElementById('doorStatus').innerText = `Door is ${status}`; 
      logDoorActivity(status);
      getDoorStatus(); 
    })
    .catch((error) => {
      console.error(`Error updating door status: `, error);
    });
}

function logDoorActivity(action) {
  const timestamp = new Date().toISOString(); // Lấy thời gian hiện tại ở định dạng ISO
  set(ref(database, `door/log/${timestamp}`), { action: action, timestamp: timestamp }) // Lưu hành động vào Firebase
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
getDoorStatus();
