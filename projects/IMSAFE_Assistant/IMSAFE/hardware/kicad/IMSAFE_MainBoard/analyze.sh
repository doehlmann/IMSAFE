#!/bin/bash
echo "=== IMSAFE MainBoard Schematic Component Analysis ==="
echo ""

for section in Section_01_MCU_Core Section_04_BMS Section_07_Barometric Section_08_IMU Section_09_Magnetometer Section_10_Gas_Sensor Section_11_Temp_Humidity Section_12_Display Section_13_microSD Section_14_IO_Expander Section_17_Bluetooth; do
  file="${section}.kicad_sch"
  if [ ! -f "$file" ]; then continue; fi
  
  echo "=== $section ==="
  
  # Extract main ICs
  main_ic=$(grep -oE 'lib_id "[^"]*:([^"\s]+)"' "$file" | grep -v "Device:\|power:" | head -1)
  if [ -n "$main_ic" ]; then
    echo "Main IC: $main_ic"
  fi
  
  # Component counts
  echo "Components:"
  grep -o 'lib_id "[^"]*"' "$file" | sort | uniq -c | awk '{print "  " $2 " (" $1 "x)"}' | head -8
  
  # Wire count
  wires=$(grep -c '^[[:space:]]*(wire' "$file" 2>/dev/null || echo 0)
  echo "Wire segments: $wires"
  echo ""
done

