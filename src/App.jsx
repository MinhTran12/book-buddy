import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import LoginPage from './LoginPage';
import LandingPage from './LandingPage';
import SettingsPage from './SettingsPage';
import MyBooksPage from './MyBooksPage';
import MyPalsPage from './MyPalsPage';

function App() {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<LoginPage/>} />
        <Route path="/landing" element={<LandingPage/>} />
        <Route path="/settings" element={<SettingsPage />} />
        <Route path="/my-books" element={<MyBooksPage />} />
        <Route path="/my-pals" element={<MyPalsPage />} />
      </Routes>
    </Router>
  );
}

export default App;
