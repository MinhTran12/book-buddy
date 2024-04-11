// import PageSelect from "./components/PageSelect";
import BLEComponent from "./esp32Moniter";

function LandingPage() {
  
    return (
      <div>
        <h1>Book Buddy</h1>
        {/* <p>Welcome to the landing page!</p>
        <PageSelect/> */}

        <BLEComponent/>
      </div>
    );
  }
  
  export default LandingPage;
