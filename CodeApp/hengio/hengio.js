let lightStatus = false;
const lightStatusElement = document.getElementById('lightStatus');

function setTimer() {
  const onTime = document.getElementById('onTime').value;
  const offTime = document.getElementById('offTime').value;

  if (onTime && offTime) {
    const now = new Date();
    const onTimeDate = new Date(now.toDateString() + ' ' + onTime);
    const offTimeDate = new Date(now.toDateString() + ' ' + offTime);

    // Kiểm tra thời gian bật và tắt
    if (onTimeDate >= offTimeDate) {
      alert('Thời gian bật phải nhỏ hơn thời gian tắt');
      return;
    }

    // Bật đèn
    const timeUntilOn = onTimeDate - now;
    if (timeUntilOn >= 0) {
      setTimeout(() => {
        lightStatus = true;
        updateLightStatus();
        sendLightCommand('turnOn');
      }, timeUntilOn);
    }

    // Tắt đèn
    const timeUntilOff = offTimeDate - now;
    if (timeUntilOff >= 0) {
      setTimeout(() => {
        lightStatus = false;
        updateLightStatus();
        sendLightCommand('turnOff');
      }, timeUntilOff);
    }

    alert('Đã đặt giờ thành công!');
  } else {
    alert('Vui lòng chọn thời gian bật và tắt đèn.');
  }
}

function updateLightStatus() {
  lightStatusElement.textContent = lightStatus ? 'Bật' : 'Tắt';
  lightStatusElement.className = lightStatus ? '' : 'off';
}

function sendLightCommand(command) {
  fetch(`http://192.168.1.100/${command}`)
    .then(response => response.text())
    .then(data => console.log(data))
    .catch(error => console.error('Lỗi:', error));
}
