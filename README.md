# RFID Attendance Management System

An enterprise-grade IoT-based RFID attendance management solution for educational institutions. This project provides a complete stack including a modern React web dashboard, backend services, and embedded firmware for RFID reader devices.

## Project Overview

The RFID Attendance Management System enables colleges and universities to:

- **Track student attendance** in real-time using RFID card readers
- **Manage device infrastructure** with ESP32-based RFID reader nodes
- **Generate attendance reports** with analytics and trends
- **Administer student records** and institutional settings
- **Monitor live attendance streams** with WebSocket/SSE integration

## Architecture

The system consists of three main components:

### 1. Frontend Dashboard (`attenda-sync-main/`)

A modern, responsive web application built with:
- **React 19** — latest React with concurrent rendering
- **TypeScript** — type-safe, maintainable code
- **Vite** — lightning-fast build tooling
- **TanStack Start/Router** — file-based routing with server-side capabilities
- **Radix UI** — accessible component primitives
- **Tailwind CSS** — utility-first styling
- **Firebase** — authentication and real-time data
- **Axios** — HTTP client with JWT support

**Key Features:**
- Dashboard with KPIs, attendance trends, and department analytics
- Student management (CRUD, search, filters, history)
- Attendance logs with CSV export
- Live attendance stream simulation
- Device status monitoring and management
- Advanced reporting and analytics
- Dark mode and responsive design
- JWT-based authentication

### 2. Backend Services

Implemented using **Google Firestore** with:
- JWT authentication via Firebase Auth
- Real-time student and attendance data synchronization
- Live attendance stream via Firestore listeners (real-time)
- Device management and heartbeat tracking
- Report generation and analytics
- Security rules for data access control

### 3. Firmware (`firmware/`)

Embedded software for ESP32-based RFID reader devices:
- RFID tag detection and parsing
- Wireless connectivity and device status
- Attendance event transmission to backend
- Status LED indicators and UI feedback
- Battery/power management

*Note: Firmware folder is currently empty. Implementation to follow.*

## Getting Started

### Prerequisites

- **Node.js** 18+ or **Bun** (recommended for faster installs)
- **npm**, **yarn**, or **bun** package manager
- Environment variables configured

### Frontend Setup

```bash
cd attenda-sync-main
bun install          # or: npm install / yarn install
bun run dev          # or: npm run dev / yarn dev
```

The application will start at `http://localhost:5173` (Vite default).

**Login Credentials:** See login page for demo credentials.

### Environment Configuration

Create a `.env` file in the `attenda-sync-main/` directory with your Firebase project credentials:

```env
VITE_FIREBASE_API_KEY=your-api-key
VITE_FIREBASE_AUTH_DOMAIN=your-project.firebaseapp.com
VITE_FIREBASE_PROJECT_ID=your-project-id
VITE_FIREBASE_STORAGE_BUCKET=your-project.appspot.com
VITE_FIREBASE_MESSAGING_SENDER_ID=your-sender-id
VITE_FIREBASE_APP_ID=your-app-id
```

## Firestore Collections & Data Model

The backend uses the following Firestore collections:

| Collection | Documents | Description |
| --- | --- | --- |
| `users` | `{userId}` | Admin and staff accounts with auth data |
| `students` | `{studentId}` | Student records (name, ID, department, etc.) |
| `attendance` | `{attendanceId}` | Individual scan records with timestamp and device info |
| `devices` | `{deviceId}` | RFID reader device metadata, location, status |
| `institutes` | `{instituteId}` | Institution settings and configuration |
| `reports` | `{reportId}` | Generated attendance summaries and analytics |

**Real-time Integration:** The frontend uses Firestore listeners for live data updates:
- Student attendance changes trigger real-time UI updates
- Device heartbeat status updates instantly
- Live attendance stream via Firestore collection listener
- No additional WebSocket configuration required

## Project Structure

```
rfid-attendance-system/
├── attenda-sync-main/          # Frontend application
│   ├── src/
│   │   ├── components/         # Reusable UI components
│   │   │   ├── common/        # Layout & common components
│   │   │   ├── layout/        # Page layout shells
│   │   │   ├── students/      # Student-specific forms
│   │   │   └── ui/            # Radix UI primitives
│   │   ├── context/           # React Context providers
│   │   ├── hooks/             # Custom React hooks
│   │   ├── routes/            # TanStack Router file-based routes
│   │   ├── services/          # API & business logic layer
│   │   ├── types/             # TypeScript type definitions
│   │   ├── lib/               # Utility functions
│   │   ├── styles.css         # Global styles
│   │   ├── router.tsx         # Router configuration
│   │   └── start.tsx          # Application entry point
│   ├── public/                # Static assets
│   ├── package.json
│   ├── tsconfig.json
│   ├── vite.config.ts
│   ├── components.json        # shadcn/ui config
│   └── README.md              # Frontend-specific docs
│
└── firmware/                   # ESP32 RFID reader firmware
    └── (TBD)
```

## Development Workflow

### Frontend Development

1. **Start dev server:** `bun run dev`
2. **Hot module replacement** — changes reflect instantly
3. **Lint:** `bun run lint`
4. **Format:** `bun run format`
5. **Build for production:** `bun run build`

### Mock vs. Real Backend

The frontend currently runs on a **mock API layer** (`src/services/mockData.ts`). To connect to the **live Firestore backend**:

1. Update `.env` with your Firebase project credentials
2. Open each service in `src/services/` (e.g., `studentService.ts`)
3. Replace mock data resolution with Firestore SDK calls:
   - Use `collection(db, 'students')` instead of mockData
   - Use `onSnapshot()` for real-time listeners
   - Use `query()` with `where()` for filtering
4. No component changes needed — types already match the Firestore schema

### Authentication Flow

- Firebase Auth handles JWT token generation and management
- Tokens automatically added to Firestore requests
- Protected routes guard unauthenticated access
- Demo credentials available on login page
- Firestore Security Rules enforce data access control

## Technology Stack

| Layer | Technology |
| --- | --- |
| **Frontend** | React 19, TypeScript, Vite |
| **Routing** | TanStack Start / TanStack Router |
| **UI Framework** | Radix UI, shadcn/ui |
| **Styling** | Tailwind CSS |
| **State Management** | React Context + Hooks |
| **HTTP Client** | Firebase SDK (Firestore & Auth) |
| **Forms** | React Hook Form + Zod |
| **Authentication** | Firebase Auth with JWT |
| **Backend** | Google Firestore (NoSQL) |
| **Database** | Firestore with Real-time Listeners |
| **Embedded** | C/C++ on ESP32 |

## Building & Deployment

### Development Build

```bash
bun run build:dev
```

### Production Build

```bash
bun run build
```

Output will be in the `dist/` folder, ready for deployment to any static hosting service (Vercel, Netlify, AWS S3, etc.).

## Performance & Security

- **Code splitting** via Vite and TanStack Router
- **Tree-shaking** for minimal bundle size
- **JWT-based authentication** with secure storage
- **HTTPS-ready** API communication
- **CSRF protection** via Axios configuration
- **Error logging** with Lovable integration

## Troubleshooting

### Firebase Connection Issues

- Verify Firebase credentials in `.env`
- Check Firebase project is active and Firestore database enabled
- Ensure Firestore Security Rules allow read/write access
- Review browser console for Firebase SDK errors

### Authentication Problems

- Clear localStorage and reload
- Check Firebase Auth configuration
- Verify user account exists in Firebase Console
- Check Firestore Security Rules for auth claims

### Build Failures

- Clear `node_modules` and reinstall: `rm -rf node_modules && bun install`
- Clear Vite cache: `rm -rf dist .vite`
- Check Node.js version: `node --version` (should be 18+)

## Contributing

1. Create a feature branch: `git checkout -b feature/your-feature`
2. Make changes and test locally
3. Run lint & format: `bun run lint && bun run format`
4. Push and create a pull request

## Documentation

- **Frontend Docs:** See [attenda-sync-main/README.md](./attenda-sync-main/README.md)
- **API Reference:** Documented inline in route files
- **Component Guide:** Check Storybook/component exports in `src/components/`
- **Type Definitions:** See `src/types/index.ts`

## License

*To be determined*

## Support & Contact

For questions or issues, please:
- Open a GitHub issue
- Contact the development team
- Review inline documentation in component files

---

**Last Updated:** 2026-08-14  
**Status:** Active Development
