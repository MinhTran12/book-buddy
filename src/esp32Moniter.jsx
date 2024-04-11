import { useState, useEffect } from 'react';
import "./css/BLEComponent.css"

const BLEComponent = () => {
  const [device, setDevice] = useState(null);
  const [characteristicForReading, setCharacteristicForReading] = useState(null);
  const [characteristicForWriting, setCharacteristicForWriting] = useState(null);
  const [characteristicForWriting2, setCharacteristicForWriting2] = useState(null);
  const [readingTime, setReadingTime] = useState(0);
  const [readingGoal, setReadingGoal] = useState(30);
  const [inputNumber, setInputNumber] = useState('');
  const [isReading, setIsReading] = useState(false);

  const connectToDevice = async () => {
    try {
      const filters = [{
        name: 'BookBuddy' 
      }];
      const device = await navigator.bluetooth.requestDevice({
        filters: filters,
        optionalServices: ['2ef8decc-c001-4e2c-8d97-6e9aa61e30e4',
        '2ef8decc-c002-4e2c-8d97-6e9aa61e30e4', 
        '2ef8decc-c005-4e2c-8d97-6e9aa61e30e4', 
        "2ef8decc-c004-4e2c-8d97-6e9aa61e30e4"] // Specify the service UUID
      });
      
      const server = await device.gatt.connect();
      const service = await server.getPrimaryService('2ef8decc-c001-4e2c-8d97-6e9aa61e30e4');
      const characteristicForWriting = await service.getCharacteristic('2ef8decc-c002-4e2c-8d97-6e9aa61e30e4');
      const characteristicForReading = await service.getCharacteristic('2ef8decc-c005-4e2c-8d97-6e9aa61e30e4');
      const characteristicForWriting2 = await service.getCharacteristic('2ef8decc-c004-4e2c-8d97-6e9aa61e30e4');

      characteristicForReading.addEventListener('characteristicvaluechanged', handleCharacteristicValueChanged);
      await characteristicForReading.startNotifications();

      setDevice(device);
      setCharacteristicForReading(characteristicForReading);
      setCharacteristicForWriting(characteristicForWriting);
      setCharacteristicForWriting2(characteristicForWriting2);
    } catch (error) {
      console.error('Error connecting to Bluetooth device:', error);
    }
  };

  const handleCharacteristicValueChanged = (event) => {
    const value = event.target.value;
    const receivedInteger = new Int32Array(value.buffer)[0];
    setReadingTime(receivedInteger);
  };

  useEffect(() => {
    return () => {
      if (characteristicForReading) {
        characteristicForReading.removeEventListener('characteristicvaluechanged', handleCharacteristicValueChanged);
        characteristicForReading.stopNotifications();
      }
    };
  }, [characteristicForReading]);

  const sendDataToDevice = async () => {
    if (!characteristicForWriting) {
      console.error('No characteristic available for writing');
      return;
    }

    try {
      const numberToSend = parseInt(inputNumber);
      setReadingGoal(inputNumber);
      await characteristicForWriting.writeValue(new TextEncoder().encode(numberToSend.toString()));
      console.log('Data sent successfully:', numberToSend);
    } catch (error) {
      console.error('Error sending data:', error);
    }
  };

  const sendStringToDevice = async (str) => {
    if (!characteristicForWriting2) {
      console.error('No characteristic available for writing string data');
      return;
    }

    try {
      await characteristicForWriting2.writeValue(new TextEncoder().encode(str));
      console.log('String data sent successfully:', str);
    } catch (error) {
      console.error('Error sending string data:', error);
    }
  };

  const calculateCircleDiameter = () => {
    const screenWidth = window.innerWidth;
    return screenWidth / 3;
  };

  const handleButtonClick = () => {
    setIsReading(!isReading);
    sendStringToDevice(isReading ? 'friendStopReading' : 'friendReading');
  };

  return (
    <div>
      <h2>Books</h2>
      <div className="book-container">
        <img src="/img/pride-and-prejudice.webp" className="book-image" />
        <img src="/img/a-tale-of-two-cities.jpg" className="book-image" />
        <img src="/img/good-omens.jpg" className="book-image" />
      </div>

      <h2>Friends</h2>
      <div className="person-container">
        <button className="status-button" onClick={handleButtonClick}>
          <img src="/img/stock-person.jpg" alt="Person" className="person-image" />
        </button>
        <p className="online-status">{isReading ? 'Alex is reading!' : 'Alex is not reading'}</p>
      </div>
      
      <h2>Goal</h2>
      
      <div className="circle-container" style={{ width: calculateCircleDiameter(), height: calculateCircleDiameter() }}>
        <div className="circle">
          <p className="message">{readingTime}/{readingGoal}</p>
        </div>
      </div>
      
      <input
        type="number"
        value={inputNumber}
        onChange={(e) => setInputNumber(e.target.value)}
        placeholder="Enter a number"
      />
      <button onClick={sendDataToDevice}>Select Reading Goal in Seconds</button>
      <button onClick={connectToDevice}>Connect to BLE Device</button>
    </div>
  );
};

export default BLEComponent;
