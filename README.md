# NvM_EraseAll: Full Erase for the 24C04 EEPROM

Erase an entire 24C04 serial EEPROM (512 bytes) page by page, with a progress/status callback so the application knows when the erase completes.

## Aim

Erase all 64 pages (512 bytes / 8 bytes per page) of a 24C04 EEPROM by running the single-page erase on each page in turn. The application is notified of progress and completion through a callback.

## Apparatus

- 24C04 serial EEPROM (4 Kbit = 512 bytes) on an I²C bus
- Microcontroller with an I²C master peripheral and a millisecond delay or timer
- C compiler (the included test uses `gcc` on a host simulation)

## Theory

- **Memory organisation:** 512 bytes ÷ 8 bytes per page = 64 pages. The 24C04 needs 9 address bits. Address bit 8 goes in the device address as P0, and bits 7-0 are sent as the word address.
- **Page size note:** the 24C04 datasheet specifies 16-byte pages. This project uses 8-byte pages, which is valid because each write stays inside a 16-byte page. For 16-byte pages, change the page-size constant; the page count becomes 32.
- **Erase:** EEPROM has no separate erase command. "Erase" means writing `0xFF` to every byte, so one page erase is one 8-byte page write.
- **Write cycle:** after each page write the chip is busy for about 5 ms (tWR) and ignores the bus. The software must wait, or ACK-poll, before the next write.
- **Total time:** 64 pages × 5 ms = **320 ms**. A blocking call this long needs a way to report progress.
- **Callback:** called after every page with a status (in progress, done or failed), the number of pages finished, and the total of 64. The last page reports "done". A write error reports "failed" with the number of pages completed.
- **Safety:** a busy flag rejects a second erase request while one is running. The erase stops at the first failed page.

## Project Structure

```
.
├── nvm_eraseall.c   # NvM_EraseAll + host simulation and test
└── README.md
```

## Build and Run (host simulation)

```bash
gcc -Wall -o nvm_eraseall nvm_eraseall.c
./nvm_eraseall
```

The program exits with code `0` if the erase succeeds and every byte reads back as `0xFF`.

## Porting to Hardware

1. Replace the simulated I²C page-write and delay routines with your MCU's I²C driver and timer.
2. Optionally replace the fixed 5 ms wait with ACK polling, which usually finishes sooner.
3. Keep the callback short. It runs in the caller's context, and `NvM_EraseAll` must not be called from inside it.
4. If a 320 ms blocking call is too long for your scheduler, convert the loop into a state machine that erases one page per main-function tick. The same callback still works.

## Procedure (test)

1. Fill the simulated EEPROM with dirty data.
2. Register the progress callback.
3. Start the full erase.
4. For each page 0 to 63: write 8 bytes of `0xFF` at address `page × 8`, wait 5 ms, call the callback.
5. After the erase returns, read back all 512 bytes and confirm each is `0xFF`.

## Output

```
[CB] progress  25% (16/64 pages, t=80 ms)
[CB] progress  50% (32/64 pages, t=160 ms)
[CB] progress  75% (48/64 pages, t=240 ms)
[CB] ERASE COMPLETE: 64/64 pages at t=320 ms
NvM_EraseAll returned 0, total time = 320 ms, non-0xFF bytes = 0
Page 63 check: eeprom[504..511] = FF FF ... FF
```

## Result

All 64 pages (512 bytes) were erased and the call returned success. The callback reported 25%, 50%, 75% and then completion. The elapsed time was 320 ms, matching 64 × 5 ms, and the read-back found no byte other than `0xFF`.

## License

MIT (or your preferred license).
