# Bank Exercise UI

A modern, responsive web-based user interface for the Bank Exercise REST API built with Vue.js 3.

## Features

- 🎨 **Beautiful UI** - Modern gradient design with smooth animations
- 📱 **Responsive** - Works on desktop, tablet, and mobile devices
- 👤 **Account Management** - Create customer and enterprise accounts
- 💰 **Banking Operations**:
  - Deposit money into accounts
  - Withdraw money from accounts
  - Transfer funds between accounts
- 🔄 **Real-time Status** - Server health indicator with auto-refresh
- ⚡ **Fast & Interactive** - Built with Vue.js 3 for smooth interactions
- 📋 **Account List** - View all your accounts with balances

## Getting Started

### Prerequisites

- The Bank Exercise server must be running on `http://localhost:8080`
- Any modern web browser (Chrome, Firefox, Safari, Edge)
- No build step required - uses Vue.js from CDN

### Running the UI

1. Open the `index.html` file in your web browser:
   - Double-click `index.html` in your file explorer, or
   - Use a local web server:
     ```bash
     # Using Python 3
     python -m http.server 8000
     # Then visit http://localhost:8000/index.html
     
     # Using Python 2
     python -m SimpleHTTPServer 8000
     
     # Using Node.js http-server
     npx http-server
     ```

2. Make sure the Bank Exercise server is running:
   ```bash
   cd /path/to/Bank_Exercise/build
   ./Bank_Exercise
   ```

3. The UI will automatically connect to the server and show a status indicator

## Usage

### Creating Accounts

1. Choose account type (Customer or Enterprise) in the left panel
2. Fill in the required information:
   - **Customer**: First Name, Last Name
   - **Enterprise**: Y-Tunnus (Business ID), Company Name
3. Click the "Create" button
4. The new account will appear in the "Your Accounts" list

### Performing Operations

1. Select an account from the "Your Accounts" list
2. Use the operations panel to:
   - **Deposit**: Enter an amount and click "Deposit"
   - **Withdraw**: Enter an amount (must not exceed balance) and click "Withdraw"
   - **Transfer**: Enter recipient account ID and amount, then click "Transfer"

### Monitoring Connection

- The status indicator in the top-right shows server connection status
- Green indicator means connected, red means disconnected
- Click "Check Status" to manually check connection
- Auto-checks connection every 5 seconds

## UI Components

### Header
- Application title and welcome message

### Status Bar
- Server connection indicator
- Manual health check button

### Create Account Panel
- Tabbed interface for Customer/Enterprise account creation
- Form validation

### Accounts Panel
- List of all created accounts
- Click to select an account
- Shows account ID, balance, and holder information
- Refresh button to reload accounts

### Operations Panel
- Shows selected account details
- Deposit operation card
- Withdraw operation card
- Transfer section

### Message Alerts
- Success messages (auto-dismiss after 5 seconds)
- Error messages (manual dismiss)
- Info messages

## API Integration

The UI communicates with these endpoints:

- `GET /health` - Check server status
- `POST /accounts/create-customer` - Create customer account
- `POST /accounts/create-enterprise` - Create enterprise account
- `GET /accounts/{accountId}` - Get account details
- `POST /accounts/{accountId}/deposit` - Deposit funds
- `POST /accounts/{accountId}/withdraw` - Withdraw funds
- `POST /accounts/transfer` - Transfer between accounts

## Styling

The UI uses:
- **Modern CSS Grid** for responsive layouts
- **Gradient backgrounds** for visual appeal
- **Smooth transitions** for interactive elements
- **Mobile-first responsive design**
- **Accessibility-friendly** color contrasts

## Troubleshooting

### Connection Issues
- Verify the Bank Exercise server is running on `http://localhost:8080`
- Check your firewall settings
- Ensure CORS is enabled on the server (check Server.cpp)

### CORS Issues
If you see CORS errors in the browser console, the server's CORS configuration may need adjustment. The server should include CORS headers:
```cpp
res.set_header("Access-Control-Allow-Origin", "*");
res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
res.set_header("Access-Control-Allow-Headers", "Content-Type");
```

### Local Server
If opening `index.html` directly doesn't work, use a local web server:
```bash
# Navigate to the UI folder
cd UI

# Python 3
python -m http.server 8000

# Then visit http://localhost:8000
```

## Browser Support

- Chrome 90+
- Firefox 88+
- Safari 14+
- Edge 90+

## Technology Stack

- **Vue.js 3** - UI framework (from CDN)
- **HTML5** - Markup
- **CSS3** - Styling with Grid and Flexbox
- **Fetch API** - HTTP requests
- **JSON** - Data format

## Notes

- All amounts are in Euros (€)
- The UI stores account data in memory (page refresh will require re-selecting accounts)
- Transfer validation prevents transfers to the same account
- Insufficient funds validation on withdrawals and transfers

## License

Same as Bank Exercise project

