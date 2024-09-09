Module.register("MMM-RoomInfo", {
  defaults: {},

  start: function () {
    this.rooms = {}; 
    this.roomOrder = []; 
  },

  getStyles: function () {
    return ["MMM-RoomInfo.css"]; 
  },

  getDom: function () {
    const wrapper = document.createElement("div");
    wrapper.className = "room-list-container"; 

    this.roomOrder.forEach((room) => {
      const roomContainer = document.createElement("div");
      roomContainer.className = "room-container";

      const roomName = document.createElement("div");
      roomName.className = "room-name";
      roomName.innerHTML = `Room ${room}`;

      const sensorData = document.createElement("div");
      sensorData.className = "sensor-data";

      const temperature = this.rooms[room].temperature !== undefined ? `${this.rooms[room].temperature}°C` : "N/A";
      const humidity = this.rooms[room].humidity !== undefined ? `${this.rooms[room].humidity}%` : "N/A";

      sensorData.innerHTML = `Temperature: ${temperature}, Humidity: ${humidity}`;

      roomContainer.appendChild(roomName);
      roomContainer.appendChild(sensorData);
      wrapper.appendChild(roomContainer);
    });

    return wrapper;
  },

  // Recive "ROOM_INFO" from MQTTbridge
  notificationReceived: function (notification, payload, sender) {
    if (notification === "ROOM_INFO") {
      console.log("Received ROOM_INFO:", payload); // Debug log

      if (typeof payload === "string") {
        try {
          payload = JSON.parse(payload); 
        } catch (e) {
          console.error("Error parsing JSON:", e);
          return;
        }
      }

      this.updateRoomData(payload); 
    }
  },


  updateRoomData: function (roomData) {
    console.log("Updating room data:", roomData); 

    if (!roomData.room || !roomData.sensors || !roomData.sensors.temperature || !roomData.sensors.humidity) {
      console.warn("Invalid room data:", roomData);
      return;
    }

    const existingIndex = this.roomOrder.indexOf(roomData.room);
    if (existingIndex !== -1) {
      this.roomOrder.splice(existingIndex, 1);
    }

    this.rooms[roomData.room] = {
      temperature: roomData.sensors.temperature,
      humidity: roomData.sensors.humidity
    };


    this.roomOrder.unshift(roomData.room);

    this.updateDom(); 
  }
});

