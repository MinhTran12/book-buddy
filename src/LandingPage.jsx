import PageSelect from "./components/PageSelect";

function LandingPage() {
  
    return (
      <div>
        <h1>Landing Page</h1>
        <p>Welcome to the landing page!</p>
        <PageSelect/>
        <div>
          <h2>Currently Reading</h2>
        </div>
        <div>
          <h2>Active Friends</h2>
        </div>
        <div>
          <h2>Goals</h2>
        </div>
      </div>
    );
  }
  
  export default LandingPage;
