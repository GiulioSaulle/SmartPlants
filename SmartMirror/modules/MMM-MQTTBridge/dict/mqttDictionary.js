var mqttHook = [
    {
      mqttTopic: "smart_mirror",
      mqttPayload: [
        {
          mqttNotiCmd: ["Command 1"]
        },
      ],
    },
  ];
var mqttNotiCommands = [
    {
      commandId: "Command 1",
      notiID: "PLANT_INFO",
    },
    {
      commandId: "Command 1",
      notiID: "ROOM_INFO",
    },
  ];

  module.exports = { mqttHook,  mqttNotiCommands};





