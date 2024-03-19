package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	hashcrc32 "hash/crc32"
	"io"
	"log"
	"os"
	"strconv"
	"strings"
	"time"

	"go.bug.st/serial"
	"go.bug.st/serial/enumerator"
)

const baudRate = 115200

//const baudRate = 57600

// -----------------------------------------------------------------------------------------

func displayProgressBar(current int, total int, startTime time.Time) {
	currentTime := time.Now()
	elapsed := currentTime.Sub(startTime)
	unitsPerTime := elapsed.Seconds() / float64(current)
	estimatedRemainingTimeSeconds := unitsPerTime * (float64(total - current))
	remainingTime := time.Duration(estimatedRemainingTimeSeconds) * time.Second

	hours, minutes, _ := int(remainingTime.Hours()), int(remainingTime.Minutes())%60, int(remainingTime.Seconds())%60

	const barWidth = 50 // Width of the progress bar in characters
	progress := float64(current) / float64(total)
	pos := int(barWidth * progress)
	bar := strings.Repeat("=", pos) + strings.Repeat(" ", barWidth-pos)
	fmt.Printf("\r[%s] %.2f%%, remaining time: %02d hours, %02d minutes     ", bar, progress*100, hours, minutes)
	if current >= total {
		fmt.Println() // New line on complete
	}
}

// -----------------------------------------------------------------------------------------

func waitForReadySignal(port serial.Port) {
	fmt.Println("Waiting for ready signal...")
	buf := make([]byte, 1)
	for {
		n, err := port.Read(buf)
		if err != nil {
			fmt.Println("Error reading from serial port: ", err)
			os.Exit(1)
		}
		if n > 0 && buf[0] == 'R' {
			fmt.Println("Received ready signal, sending next chunk...")
			break
		}
		time.Sleep(100 * time.Millisecond) // Small delay to prevent tight looping
	}
}

// -----------------------------------------------------------------------------------------

func programAll(firstIcOnly *bool, port serial.Port, inputFilename *string) {
	port.ResetOutputBuffer()
	println("Programming ICs:")

	data, err := os.ReadFile(*inputFilename)
	if err != nil {
		fmt.Println("Error reading file: ", err)
		os.Exit(1)
	}
	/*
		file, err := os.Open(*inputFilename)
		if err != nil {
			log.Fatalf("Failed to open file: %v", err)
		}
		defer file.Close()
	*/
	startTime := time.Now()
	/*
		_, err = port.Write([]byte("er all\n"))
		if err != nil {
			log.Fatalf("Error sending command: %v", err)
		}
		time.Sleep(100 * time.Millisecond)
	*/
	writeCommand := "wr all\n"

	if *firstIcOnly {
		writeCommand = "wr1 all\n"
	}
	_, err = port.Write([]byte(writeCommand))
	if err != nil {
		log.Fatalf("Error sending command: %v", err)
	}

	var wordsPerIc int = 8388608 * 4

	if *firstIcOnly {
		wordsPerIc = 8388608 * 2
		//wordsPerIc = 1024
	}

	//	buf := make([]byte, 2) // Buffer for reading 2 bytes at a time

	//waitForReadySignal(port)
	for i := 0; i < wordsPerIc; i += 512 {
		//waitForReadySignal(port)
		end := i + 512
		if end > len(data) {
			end = len(data)
		}
		displayProgressBar(i, wordsPerIc, startTime)

		port.Write(data[i:end])
		time.Sleep(100 * time.Millisecond)
	}

	endTime := time.Now()
	elapsed := endTime.Sub(startTime)
	hours, minutes, seconds := int(elapsed.Hours()), int(elapsed.Minutes())%60, int(elapsed.Seconds())%60

	fmt.Printf("Elapsed Time: %d hours, %d minutes, %d seconds\n", hours, minutes, seconds)
}

// -----------------------------------------------------------------------------------------

func dumpAll(firstIcOnly *bool, port serial.Port, separateIcDumps *bool, outputFilename *string) {
	port.ResetInputBuffer()

	var dumpCommand string
	dumpCommand = "dp all\n"
	if *firstIcOnly {
		dumpCommand = "dp1 all\n"
	}
	_, err := port.Write([]byte(dumpCommand))
	if err != nil {
		log.Fatalf("Error sending command: %v", err)
	}

	const icSize = 16777216
	var dataBuffer bytes.Buffer

	startTime := time.Now()

	var filename_all string

	for icIndex := 0; icIndex < 2; icIndex++ {
		if *firstIcOnly && icIndex > 0 {
			return
		}

		dataBuffer.Reset()

		if *firstIcOnly {
			fmt.Printf("Dumping IC %d of 1: \n", icIndex+1)
		} else {
			fmt.Printf("Dumping IC %d of 2: \n", icIndex+1)
		}
		startTimeIc := time.Now()
		for i := 0; i < icSize; i++ {
			displayProgressBar(i+1, icSize, startTimeIc)
			buf := make([]byte, 1)

			_, err := port.Read(buf)

			if err != nil {
				log.Fatalf("Error reading from serial port: %v", err)
			}

			dataBuffer.Write(buf)
		}

		// Determine the filename for saving
		var filename string
		if *separateIcDumps {
			filename = fmt.Sprintf("%s.ic%d", *outputFilename, icIndex+1)
		} else {
			filename = *outputFilename
			filename_all = filename
		}

		var file *os.File
		var err error
		// Open the file in append mode or create it if it doesn't exist
		if icIndex == 0 {
			file, err = os.OpenFile(filename, os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0644)
			if err != nil {
				log.Fatalf("Error opening file: %v", err)
			}

		} else {
			file, err = os.OpenFile(filename, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
			if err != nil {
				log.Fatalf("Error opening file: %v", err)
			}

		}

		// Write the buffer's contents to the file
		_, err = file.Write(dataBuffer.Bytes())
		if err != nil {
			log.Fatalf("Error writing to file: %v", err)
		}
		err = file.Close()
		if err != nil {
			log.Fatalf("Error closing file: %v", err)
		}

		if *separateIcDumps {
			file, err := os.Open(filename)
			if err != nil {
				log.Fatalf("Error opening file for CRC calculation: %v", err)
			}

			// Create a CRC32 hash calculator
			hash := hashcrc32.NewIEEE()

			// Copy the file content to the hash calculator
			if _, err := io.Copy(hash, file); err != nil {
				log.Fatalf("Error computing CRC32: %v", err)
			}

			// Calculate the CRC32 checksum
			checksum := hash.Sum32()
			fmt.Printf("CRC32 Checksum of %s: 0x%08X\n", filename, checksum)
			err = file.Close()
			if err != nil {
				log.Fatalf("Error closing file: %v", err)
				return
			} // Ensure the file is closed after computing the CRC
		}
	}

	if !*separateIcDumps {
		file, err := os.Open(filename_all)
		if err != nil {
			log.Fatalf("Error opening file for CRC calculation: %v", err)
		}

		// Create a CRC32 hash calculator
		hash := hashcrc32.NewIEEE()

		// Copy the file content to the hash calculator
		if _, err := io.Copy(hash, file); err != nil {
			log.Fatalf("Error computing CRC32: %v", err)
		}

		// Calculate the CRC32 checksum
		checksum := hash.Sum32()
		fmt.Printf("CRC32 Checksum of %s: 0x%08X\n", filename_all, checksum)
		err = file.Close()
		if err != nil {
			log.Fatalf("Error closing file: %v", err)
			return
		} // Ensure the file is closed after computing the CRC
	}

	endTime := time.Now()
	elapsed := endTime.Sub(startTime)
	hours, minutes, seconds := int(elapsed.Hours()), int(elapsed.Minutes())%60, int(elapsed.Seconds())%60

	fmt.Printf("Elapsed Time: %d hours, %d minutes, %d seconds\n", hours, minutes, seconds)
}

// -----------------------------------------------------------------------------------------

func eraseAll(port serial.Port) {
	port.ResetOutputBuffer()

	var dumpCommand string
	dumpCommand = "er all\n"

	_, err := port.Write([]byte(dumpCommand))
	if err != nil {
		log.Fatalf("Error sending command: %v", err)
	}

	println("Erasing flash. This will take a few seconds...")
	buffer := make([]byte, 128) // Buffer for incoming data
	var incomingData string     // To hold data read from port
	for {
		n, err := port.Read(buffer)
		if err != nil {
			log.Fatalf("Error reading from port: %v", err)
		}
		if n == 0 {
			log.Println("No data received, waiting...")
			time.Sleep(time.Second) // Wait for a bit before trying to read again
			continue
		}

		// Append the incoming data to a string
		incomingData += string(buffer[:n])

		// Check if the incoming data string contains "done"
		if strings.Contains(incomingData, "done") {
			break // Exit the loop
		}
	}
	println("Done.")
}

// -----------------------------------------------------------------------------------------

func hexDump(port serial.Port, startAddress *int, length *int, ic *int) {
	port.ResetOutputBuffer()

	var dumpCommand string

	dumpCommand = "hdp1 " + strconv.Itoa(*startAddress) + " " + strconv.Itoa(*length) + "\n"
	if *ic == 2 {
		dumpCommand = "hdp2 " + strconv.Itoa(*startAddress) + " " + strconv.Itoa(*length) + "\n"
	}

	_, err := port.Write([]byte(dumpCommand))
	if err != nil {
		log.Fatalf("Error sending command: %v", err)
	}
	const bufferSize = 128
	buffer := make([]byte, bufferSize)

	// Set a read timeout if your serial library supports it
	// Adjust the timeout duration based on your specific requirements
	err = port.SetReadTimeout(500 * time.Millisecond)
	if err != nil {
		return
	}

	for {
		n, err := port.Read(buffer)
		if err != nil {
			log.Fatalf("Error reading from port: %v", err)
		}
		if n == 0 { // No more data, exit the loop
			break
		}

		// Print the received data to the command line
		fmt.Printf("%s", buffer[:n])
	}

}

// -----------------------------------------------------------------------------------------

func crc32(port serial.Port, ic *int) {
	port.ResetOutputBuffer()

	var dumpCommand string

	dumpCommand = "crc all\n"
	if *ic == 1 {
		dumpCommand = "crc1 all\n"
	}

	_, err := port.Write([]byte(dumpCommand))
	if err != nil {
		log.Fatalf("Error sending command: %v", err)
	}

	buffer := make([]byte, 128) // Buffer for incoming data
	var incomingData string     // To hold data read from port
	for {
		n, err := port.Read(buffer)
		if err != nil {
			log.Fatalf("Error reading from port: %v", err)
		}
		if n == 0 {
			log.Println("No data received, waiting...")
			time.Sleep(time.Second) // Wait for a bit before trying to read again
			continue
		}

		// Append the incoming data to a string
		incomingData += string(buffer[:n])
		fmt.Printf("%s", buffer[:n])
		// Check if the incoming data string contains "done"
		if strings.Contains(incomingData, "done") {
			break // Exit the loop
		}
	}
}

// -----------------------------------------------------------------------------------------

func skipWelcomeString(port serial.Port) bool {
	reader := bufio.NewReader(port)
	line, err := reader.ReadString('\n')
	if err != nil {
		log.Fatalf("Error reading welcome string: %v", err)
	}
	if strings.TrimSpace(line) != strings.TrimSpace("Welcome to the SRX programmer!") {
		fmt.Println("Handshake failed. Please remove your SRX programmer from the USB port, plug it in again and retry.")
		fmt.Printf("Detected handshake string: %s\n", string(line))
		return true
	}
	return false
}

// -----------------------------------------------------------------------------------------

func printExamples() {
	fmt.Println()
	fmt.Println("Examples:")
	fmt.Println("srxtool -devices")
	fmt.Println("srxtool -mode dump -serialport /dev/ttyACM0 -out image.bin [-separateicdumps] [-firsticonly]")
	fmt.Println("srxtool -mode program -serialport /dev/ttyACM0 -in image.bin [-firsticonly]")
	fmt.Println("srxtool -mode erase -serialport /dev/ttyACM0")
	fmt.Println("srxtool -mode hexdump -serialport /dev/ttyACM0 [-start 0] [-length 64] [-ic 1]")
	fmt.Println("srxtool -mode crc32 -serialport /dev/ttyACM0 [-ic 1]")
}

// =========================================================================================

func main() {
	// Define flags
	mode := flag.String("mode", "", "mode, either 'dump', 'program', 'erase', 'hexdump', 'crc32'")
	outputFilename := flag.String("out", "", "The filename where the data will be written")
	inputFilename := flag.String("in", "", "The filename where the data to be programmed will be read from")
	serialPortName := flag.String("serialport", "", "Serial port name, e.g. /dev/ttyACM0 (Linux) or COM10 (Windows)")
	listDevices := flag.Bool("devices", false, "Lists the available serial devices. Look for VID 2341 and PID 003d!")
	separateIcDumps := flag.Bool("separateicdumps", false, "Set this to true if you want two files for each flash IC")
	firstIcOnly := flag.Bool("firsticonly", false, "Set this to true if you want to dump only the first IC")
	startAddress := flag.Int("start", 0, "start address for the hexdump")
	hexdumpLength := flag.Int("length", 64, "length of hexdump starting from the start address")
	ic := flag.Int("ic", 0, "ic number to hexdump. Can either 1 or 2.")

	help := flag.Bool("help", false, "Display help")

	// Parse the flags
	flag.Parse()

	// Check if help was requested
	if *help {
		fmt.Println("Usage of the program:")
		flag.PrintDefaults() // Print default help messages
		printExamples()
		return
	}

	if *listDevices {
		println("Look for a device with VID 2341 and PID 003e. Make sure you have the Arduino Due plugged into the native USB port!")
		println("")
		ports, err := enumerator.GetDetailedPortsList()
		if err != nil {
			log.Fatalf("Error listing serial ports: %v", err)
		}

		if len(ports) == 0 {
			fmt.Println("No serial ports found!")
			return
		}

		for _, port := range ports {
			if port.IsUSB {
				fmt.Printf("Port: %s\n", port.Name)
				fmt.Printf("\tUSB Vendor ID (VID): %s\n", port.VID)
				fmt.Printf("\tUSB Product ID (PID): %s\n", port.PID)
				fmt.Printf("\tSerial Number: %s\n", port.SerialNumber)
				fmt.Printf("\tProduct: %s\n", port.Product)
			}
		}

		return
	}

	if *mode != "dump" && *mode != "program" && *mode != "erase" && *mode != "hexdump" && *mode != "crc32" {
		println("Error: -mode parameter must be provided with either a value 'dump', 'program', 'erase', 'hexdump', 'crc32'")
		println("")
		println("")
		flag.Usage()
		printExamples()
		return
	}

	if *mode == "hexdump" && !(*ic == 0 || *ic == 1 || *ic == 2) {
		println("Error: -ic must be either 1 or 2")
		println("")
		println("")
		flag.Usage()
		printExamples()
		return
	}

	if *mode == "crc32" && !(*ic == 0 || *ic == 1) {
		println("Error: -ic must be either 0 or 1. 0 refers to a crc32 checksum over both ICs. 1 refers to a checksum over the first IC only.")
		println("")
		println("")
		flag.Usage()
		printExamples()
		return
	}

	if *mode == "hexdump" && *startAddress < 0 {
		println("Error: -startaddress must be a number 0 or higher. Each address yields a 16bit value.")
		println("")
		println("")
		flag.Usage()
		printExamples()
		return
	}

	if *mode == "hexdump" && *hexdumpLength < 0 {
		println("Error: -length must be a number 0 or higher. It specifies the amounts of addresses to hexdump.")
		println("")
		println("")
		flag.Usage()
		printExamples()
		return
	}

	if *mode == "dump" && *outputFilename == "" {
		fmt.Fprintf(os.Stderr, "Error: The 'out' filename is required.\n\n")
		flag.Usage()
		printExamples()
		return
	}

	if *mode == "program" && *inputFilename == "" {
		fmt.Fprintf(os.Stderr, "Error: The 'in' filename is required.\n\n")
		flag.Usage()
		printExamples()
		return
	}

	if *serialPortName == "" {
		fmt.Fprintf(os.Stderr, "Error: The 'serialport' parameter is required.\n\n")
		flag.Usage()
		printExamples()
		return
	}

	// Scan for available serial ports
	ports, err := enumerator.GetDetailedPortsList()
	if err != nil {
		log.Fatalf("Error listing serial ports: %v", err)
	}

	if len(ports) == 0 {
		fmt.Println("No serial ports found!")
		return
	}

	var arduinoPort string
	for _, port := range ports {
		if port.IsUSB && port.Name == *serialPortName {
			arduinoPort = port.Name
			fmt.Printf("Arduino Due found on port %s\n", arduinoPort)
			break
		}
	}

	if arduinoPort == "" {
		log.Fatalf("Arduino Due not found")
		return
	}

	println("")
	println("If nothing happens and the software stalls, please exit the program with ctrl-c, reset the Arduino Due with its reset button and try again.")
	println("")

	// establish real connection
	serialMode := &serial.Mode{
		BaudRate: baudRate, /*
			DataBits:          8,
			Parity:            serial.NoParity,
			StopBits:          serial.OneStopBit,
			InitialStatusBits: &serial.ModemOutputBits{RTS: false},*/
	}

	port, err := serial.Open(arduinoPort, serialMode)
	if err != nil {
		log.Fatalf("Error opening serial port: %v", err)
	}
	defer port.Close()
	/*
		port.SetDTR(false)
		port.SetRTS(false)
		port.SetReadTimeout(serial.NoTimeout)
	*/
	/*
		// Setup cleanup on Ctrl-C
		c := make(chan os.Signal, 1)
		signal.Notify(c, os.Interrupt, syscall.SIGTERM)
		go func() {
			<-c
			fmt.Println("\r- Ctrl-C pressed, closing serial port...")
			if err := port.Close(); err != nil {
				fmt.Println("Error closing serial port: ", err)
			} else {
				fmt.Println("Serial port closed successfully.")
			}
			os.Exit(0)
		}()
	*/
	/*
		shouldReturn := skipWelcomeString(port)
		if shouldReturn {
			return
		}
	*/
	if *mode == "dump" {
		dumpAll(firstIcOnly, port, separateIcDumps, outputFilename)
		return
	} else if *mode == "program" {
		programAll(firstIcOnly, port, inputFilename)
	} else if *mode == "erase" {
		eraseAll(port)
	} else if *mode == "hexdump" {
		hexDump(port, startAddress, hexdumpLength, ic)
	} else if *mode == "crc32" {
		crc32(port, ic)
	}
}
