# ERC Fix Guide - Manual Steps Required

## Current Status
- **Total Violations:** 390 (129 errors, 261 warnings)
- **Target:** 0 errors (warnings can be waived with justification)

## Why Manual Fixes Are Required

KiCad ERC errors require precise pin coordinate matching. Adding no-connect flags or fixing wire connections programmatically often results in:
- `no_connect_dangling` errors (no-connect not at valid pin location)
- `no_connect_connected` errors (no-connect at already-connected pin)

The safest approach is to fix these in KiCad's schematic editor.

---

## Step-by-Step Fix Process

### 1. Open Project in KiCad
```
Open: IMSAFE/hardware/kicad/IMSAFE_MainBoard/IMSAFE_MainBoard.kicad_sch
```

### 2. Run ERC
- Menu: **Inspect > Electrical Rules Checker**
- Click **Run ERC**

### 3. Fix Errors by Type

#### A. Pin Not Connected (68 errors)
For each unconnected pin:
1. Click the error in ERC list to navigate to it
2. If the pin should NOT be connected (NC pin, unused feature):
   - Press **Q** to add a no-connect flag at the pin
3. If the pin SHOULD be connected:
   - Add the missing wire connection

**Sections with NC pins to mark:**
| Section | Component | NC Pins |
|---------|-----------|---------|
| 03 Charger | BQ25792 | QON (pin 12) - optional output |
| 08 IMU | ICM-42688-P | RESV pins 8, 10, 14; INT2 (pin 11) if unused |
| 09 Magnetometer | MMC5983MA | NC pins 11-14 |
| 11 Temp/Humidity | Si7021-A20 | NC pin 3 |

#### B. Unconnected Wire Endpoint (38 errors)
1. Click error to navigate
2. Either:
   - Delete the dangling wire segment (select and press Delete)
   - Extend wire to connect to a pin or junction

#### C. Wire Dangling (34 errors)
Same as above - delete incomplete wires or complete the connection.

#### D. Label Dangling (22 errors)
1. Click error to navigate
2. Either:
   - Connect the label to a wire/pin
   - Delete the unused label

#### E. Global Label Dangling (12 errors)
1. These are hierarchical labels not used elsewhere
2. Either connect them or remove if not needed

#### F. Power Pin Not Driven (2 errors)
1. Add PWR_FLAG symbol to power input pins
2. Or connect the power pin to a power source

#### G. Pin to Pin (1 error)
1. Review the connection between two output pins
2. This is likely in the Charger section (QON vs regulator output)
3. May need circuit redesign or one pin marked as NC

---

## Verification Steps

1. After fixing, run ERC again
2. Confirm **0 errors**
3. For remaining **warnings**, add justification:
   - `lib_symbol_mismatch` - Custom symbols for JLCPCB parts (intentional)
   - `same_local_global_label` - Hierarchical design pattern (intentional)

4. Export updated schematic PDF:
   - Menu: **File > Plot > PDF**

---

## Sections Status After Programmatic Analysis

| Section | Errors | Primary Issues |
|---------|--------|----------------|
| 01 MCU Core | 11 | Missing junctions at wire intersections |
| 02 USB-C | 0 | Clean |
| 03 Charger | 6+ | BQ25792 NC pins, pin-to-pin conflict |
| 04 BMS | 11 | Missing junctions |
| 05 3V3 Reg | 9 | TPS62133 NC pins |
| 06 5V Reg | 9 | TPS62130 NC pins |
| 07 Barometric | 7 | Wire connections |
| 08 IMU | 9 | ICM-42688-P RESV pins |
| 09 Magnetometer | 9 | MMC5983MA NC pins |
| 10 Gas Sensor | 9 | Wire connections |
| 11 Temp/Humidity | 10 | Si7021-A20 NC pin |
| 12-20 | Various | Labels, wire endpoints |

---

## Quick Reference: KiCad Shortcuts

| Action | Shortcut |
|--------|----------|
| Add No-Connect | Q |
| Add Wire | W |
| Add Junction | J |
| Delete | Del |
| Zoom to Fit | Home |
| Run ERC | (Inspect > ERC) |

---

## After Fixes Complete

1. Commit the fixed schematics:
   ```bash
   git add IMSAFE/hardware/kicad/IMSAFE_MainBoard/*.kicad_sch
   git commit -m "Fix ERC errors across all schematic sections"
   ```

2. Re-export PDF and ERC report:
   ```bash
   kicad-cli sch erc --output erc-report.json --format json IMSAFE_MainBoard.kicad_sch
   kicad-cli sch export pdf -o IMSAFE_MainBoard_Schematic.pdf IMSAFE_MainBoard.kicad_sch
   ```
