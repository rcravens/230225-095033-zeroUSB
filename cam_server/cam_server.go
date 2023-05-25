//
// Camera server
//
// This provides an http server to up and download images.
// It is intended to be uses with a camrea capable pushing 
// to an API.  
//
// Example: 
//   curl -F "<secureFormFile>=@photo.jpg" <host>:8080/upload
//
// The Form File variable is used to provide a secret key used
// to prevent anyone from uploading files.
package main

import (
	"flag"
	"fmt"
	"io"
	"log/syslog"
	"net/http"
	"os"
	"path/filepath"
)

// HTTP Form File
// This is used to security this should be a 32 char random number
// See Http.cpp func post_file_buffer for the matching string.
var secureFormFile string = "JrRdd728xJ5ewKT3Cg28dFYuwgbFXKU8"

// Print test output to console
var debug bool // make debug a global variable

// Directory to server files from.
const uploadDirectory = "/images/"

var log *syslog.Writer

func main() {
	flag.BoolVar(&debug, "debug", false, "Enable debug logging")
	flag.Parse()

	fmt.Println("Camera file server. V1.2.0")
	fmt.Println("Ready to receive file")

	// Setting up syslog writer
	log, err := syslog.New(syslog.LOG_NOTICE, "cam-server")
	if err != nil {
		log.Crit(fmt.Sprintf("Can't connect to syslog: %v", err))
		return
	}
	log.Info("Camera file server started")

	// HTTP Namespace
	http.HandleFunc("/upload/", uploadHandler)
	http.HandleFunc("/download/", downloadHandler)
	error := http.ListenAndServe(":8080", nil)
	if error != nil {
		log.Crit("Unable to open port 8080! In use?")
		fmt.Println("Unable to opne port 8080! In use?")
	}
	fmt.Println("Listening on http://torks.org:8080")
}

func uploadHandler(w http.ResponseWriter, r *http.Request) {
	// Process the incoming file
	if r.Method != http.MethodPost {
		if debug {
			fmt.Println("Must use POST Method")
			http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		}
		return
	}

	// Process the incoming file
	file, handler, err := r.FormFile(secureFormFile)
	if err != nil {
		log.Info("Failed to receive file")
		if debug 
			fmt.Println("Failed to receive file")
			http.Error(w, "Failed to receive file", http.StatusBadRequest)
		}
		return
	}
	defer file.Close()

	// Create the destination file
	dst, err := os.Create(filepath.Join(uploadDirectory, handler.Filename))
	if err != nil {
		log.Info(fmt.Sprintf("Failed to create %v\n", filepath.Join(uploadDirectory, handler.Filename)))
		if debug {
			fmt.Printf("Failed to create %v\n", filepath.Join(uploadDirectory, handler.Filename))
			http.Error(w, "Failed to create file on server", http.StatusInternalServerError)
		}
		return
	}
	defer dst.Close()

	// Copy the uploaded file to the destination file
	_, err = io.Copy(dst, file)
	if err != nil {
		log.Info("Failed to save file")
		if debug {
			fmt.Println("Failed to save file")
			http.Error(w, "Failed to save file on server", http.StatusInternalServerError)
		}
		return
	}
	log.Info(fmt.Sprintf("Saved file %v\n", filepath.Join(uploadDirectory, handler.Filename)))
	if debug {
		fmt.Printf("Saved file %v\n", filepath.Join(uploadDirectory, handler.Filename))
	}

	log.Info("File upload succsessfull")
	if debug {
		fmt.Fprintf(w, "File uploaded successfull")
	}
}

func downloadHandler(w http.ResponseWriter, r *http.Request) {
	// Process a file request
	if r.Method != http.MethodGet {
		if debug {
			fmt.Println("Must use GET method")
			http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		}
		return
	}

	// Open the file requested
	filePath := filepath.Join(uploadDirectory, r.URL.Path[len("/download/"):])
	file, err := os.Open(filePath)
	if err != nil {
		log.Info(fmt.Sprintf("%v Not found\n", filePath))
		if debug {
			fmt.Printf("%v Not found\n", filePath)
			http.Error(w, "File not found", http.StatusNotFound)
		}
		return
	}
	defer file.Close()

	// Set the appropriate headers for a JPG image
	w.Header().Set("Content-Type", "image/jpeg")
	w.Header().Set("Content-Disposition", "inline; filename="+filepath.Base(filePath))

	// Copy the file to the response writer
	_, err = io.Copy(w, file)
	if err != nil {
		log.Info("Failed to send file")
		fmt.Println("Failed to send file")
		http.Error(w, "Failed to download file", http.StatusInternalServerError)
		return
	}

	log.Info(fmt.Sprintf("Sent %v", filePath))
	if debug {
		fmt.Printf("Sent %v\n", filePath)
	}
}
