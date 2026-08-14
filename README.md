# RFID Attendance Management System — Frontend

An IoT-based RFID attendance dashboard for colleges. Built with React 19, TypeScript, Vite and TanStack Start/Router. It currently runs on a mock API layer and is structured to swap onto a Spring Boot REST backend with no component changes.

## Features

- **Dashboard** — live KPIs, weekly attendance area chart, distribution pie, department bar chart, recent scans
- **Students** — paginated list with search/filters, add, edit, delete, detail page with per-student history
- **Attendance** — filterable log with CSV export
- **Live Attendance** — simulated real-time scan stream (drop-in replacement for WebSocket/SSE)
- **Devices** — ESP32 reader status, heartbeat and location management
- **Reports** — department comparisons and trend analytics
- **Settings / Profile** — institute config, RFID behaviour, notifications, admin account
- **Dark mode**, responsive layout, toast notifications, JWT-ready auth guard

## Getting started

```bash
bun install
bun run dev
```

Login with the demo credentials shown on the login page.

## Environment

Copy `.env.example` to `.env`:

```
VITE_API_BASE_URL=http://localhost:8080/api
```

## Connecting the Spring Boot backend

All HTTP access goes through `src/services/api.ts` (Axios instance with a JWT request interceptor and normalized error handling). Each domain service (`studentService`, `attendanceService`, `deviceService`, `reportService`, `authService`) currently resolves mock data from `src/services/mockData.ts`.

To go live, replace the mock resolution inside each service with the matching `api` call — the exported function signatures and the types in `src/types/index.ts` already mirror the expected REST contract:

| Method | Endpoint |
| --- | --- |
| POST | `/auth/login` |
| GET/POST | `/students` |
| GET/PUT/DELETE | `/students/{id}` |
| GET | `/attendance` |
| GET | `/attendance/live` (WebSocket/SSE) |
| GET/POST | `/devices` |
| GET | `/reports/summary` |

## Project structure

```
src/
  components/   shared UI + layout
  context/      Auth and Theme providers
  routes/       file-based TanStack routes
  services/     API layer + mock data
  types/        shared TypeScript contracts
```
