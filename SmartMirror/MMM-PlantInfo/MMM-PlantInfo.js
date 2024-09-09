Module.register("MMM-PlantInfo", {
  defaults: {
  },

  start: function () {
    this.plants = []; 
  },

  getStyles: function () {
    return ["MMM-PlantInfo.css"];
  },

  getDom: function () {
    const wrapper = document.createElement("div");
    wrapper.className = "plant-list-container";

    this.plants.forEach((plant) => {
      const plantContainer = document.createElement("div");
      plantContainer.className = "plant-container";

      const image = document.createElement("img");
      image.src = plant.plant_img; 
      image.alt = plant.plant; 
      image.className = "image";
      image.onerror = function () { 
        this.src = "https://via.placeholder.com/100"; 
      };

      const textContainer = document.createElement("div");
      textContainer.className = "text";

      const plantName = document.createElement("div");
      plantName.className = "plant-name";
      plantName.innerHTML = `Plant: ${plant.plant}`;

      const wateringTime = document.createElement("div");
      //wateringTime.innerHTML = `Watering time: ${plant.watering_time} sec`;

      const message = document.createElement("div");
      message.className = "message";
      message.innerHTML = `Says: ${plant.message}`;

      const sensorData = document.createElement("div");
      sensorData.className = "sensor-data";
      //add
      //        <strong>Soil Moisture:</strong> ${plant.sensors.soil_moisture},
      //to show moisture
      sensorData.innerHTML = `
        <p>
        <strong>Temperature:</strong> ${plant.sensors.temperature} °C, 
        <strong>Humidity:</strong>  ${plant.sensors.humidity}%, 
        <strong>Light:</strong>  ${plant.sensors.light} Lux</p>
      `;

      textContainer.appendChild(plantName);
      textContainer.appendChild(wateringTime);
      textContainer.appendChild(message);
      textContainer.appendChild(sensorData);

      plantContainer.appendChild(image);
      plantContainer.appendChild(textContainer);

      wrapper.appendChild(plantContainer);
    });

    return wrapper;
  },

  notificationReceived: function (notification, payload, sender) {
    if (notification === "PLANT_INFO") {
      console.log("Ricevuto PLANT_INFO:", payload); 
 
      if (typeof payload === "string") {
        try {
          payload = JSON.parse(payload); 
        } catch (e) {
          console.error("Errore nel parsing del JSON:", e);
          return;
        }
      }

      this.updatePlantData(payload); 
    }
  },

  updatePlantData: function (plantData) {
    console.log("Aggiornamento dati pianta:", plantData); 
    const plantListContainer = document.querySelector(".plant-list-container");

    if (!plantData.plant) {
      console.warn("Il campo 'plant' è mancante o invalido:", plantData.plant);
      return;
    }
    if (!plantData.plant_img) {
      console.warn("Il campo 'plant_img' è mancante o invalido:", plantData.plant_img);
      return;
    }
    if (!plantData.watering_time) {
      console.warn("Il campo 'watering_time' è mancante o invalido:", plantData.watering_time);
      return;
    }
    if (!plantData.sensors || !plantData.sensors.soil_moisture || !plantData.sensors.temperature || !plantData.sensors.humidity || !plantData.sensors.light) {
      console.warn("I campi 'sensors' sono mancanti o invalidi:", plantData.sensors);
      return;
    }
    if (!plantData.message) {
      console.warn("Il campo 'message' è mancante o invalido:", plantData.message);
      return;
    }

    const existingPlantIndex = this.plants.findIndex(plant => plant.plant === plantData.plant);

    if (existingPlantIndex !== -1) {
      this.plants[existingPlantIndex] = plantData; 
    } else {
      this.plants.unshift(plantData);
    }
    
    if (this.plants.length > 0) {
      plantListContainer.classList.add("has-data");
    } else {
      plantListContainer.classList.remove("has-data");
    }

    this.updateDom(); 

    if (this.plants.length > 0) {
      const lastPlant = document.querySelector('.plant-container:last-child');
      if (lastPlant) {
        lastPlant.scrollIntoView({ behavior: 'smooth', block: 'end' });
      }
    }
  }
});

