import { useNavigate } from 'react-router-dom';

function PageSelect() {
  const navigate = useNavigate();

  const handlePageChange = (event) => {
    const selectedOption = event.target.value;
    navigate(selectedOption);
  };

  return (
    <div>
      <label htmlFor="page-select">Select a page:</label>
      <select id="page-select" onChange={handlePageChange}>
        <option value="/landing">Landing Page</option>
        <option value="/settings">Settings</option>
        <option value="/my-books">My Books</option>
        <option value="/my-pals">My Pals</option>
      </select>
    </div>
  );
}

export default PageSelect;
