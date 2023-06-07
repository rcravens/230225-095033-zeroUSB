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
	"log"
	"net/http"
	"os"
	"path/filepath"
	"time"
)

// HTTP Form File
// This is used to security this should be a 32 char random number
// See Http.cpp func post_file_buffer for the matching string.
var secureFormFile string = "JrRdd728xJ5ewKT3Cg28dFYuwgbFXKU8"

// Print test output to console
var debug bool // make debug a global variable
var crt string // Path and filename for TLS certificate
var key string // Path and filename for TLS key

// Directory to server files from.
const uploadDirectory = "/images"
const archiveDirectory = "/images/archive"

func main() {

	flag.BoolVar(&debug, "debug", false, "Enable debug logging")
	flag.StringVar(&crt, "crt", "", "Path and filename for TLS certificate")
	flag.StringVar(&key, "key", "", "Path and filename for TLS Key")
	flag.Parse()

	fmt.Println("Camera file server. V1.2.1")
	log.Println("Ready to recevie files")

	// HTTP Namespace
	http.HandleFunc("/upload", uploadHandler)
	http.HandleFunc("/download/", downloadHandler)

	go func() {
		if error := http.ListenAndServe(":8080", nil); error != nil {
			log.Fatalf("Unable to open port 8080! Error: %v\n", error)
		} else {
			if debug {
				log.Println("8080 Open")
			}
		}
	}()

	if crt != "" {
		go func() {
			if error := http.ListenAndServeTLS(":8443", crt, key, nil); error != nil {
				log.Printf("Unable to open port 8443! %v\n", error)
			} else {
				if debug {
					log.Println("8443 TLS Open")
				}
			}
		}()
	}

	if debug {
		log.Println("Lissioning on 8080")
	}
	select {}
}

func uploadHandler(w http.ResponseWriter, r *http.Request) {

	currentTime := time.Now()
	fileTime := fmt.Sprintf("%4d-%02d-%02dT%02d:%02d:%02d-",
		currentTime.Year(),
		currentTime.Month(),
		currentTime.Day(),
		currentTime.Hour(),
		currentTime.Hour(),
		currentTime.Second())

	// Process the incoming file
	if r.Method != http.MethodPost {
		if debug {
			log.Println("Must use POST Method")
		}
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	// Process the incoming file
	file, handler, err := r.FormFile(secureFormFile)
	if err != nil {
		ip := r.RemoteAddr
		http.Error(w, "Failed to receive file", http.StatusBadRequest)
		log.Printf("Failed to receive file from %v", ip)
		return
	}
	defer file.Close()

	// Create the destination file
	dst, err := os.Create(filepath.Join(uploadDirectory, handler.Filename))
	if err != nil {
		if debug {
			log.Printf("Failed to create %v\n", filepath.Join(uploadDirectory, handler.Filename))
			http.Error(w, "Failed to create file on server", http.StatusInternalServerError)
		}
		return
	}
	defer dst.Close()

	// Create archive destination file
	adst, err := os.Create(filepath.Join(archiveDirectory, fileTime+handler.Filename))
	if err != nil {
		if debug {
			log.Printf("Failed to create %v\n", filepath.Join(archiveDirectory, fileTime+handler.Filename))
			http.Error(w, "Failed to create file on server", http.StatusInternalServerError)
		}
		return
	}
	defer adst.Close()

	// Copy the uploaded file to the destination file
	_, err = io.Copy(dst, file)
	if err != nil {
		if debug {
			log.Println("Failed to save file")
			http.Error(w, "Failed to save file on server", http.StatusInternalServerError)
		}
		return
	}
	if debug {
		fmt.Printf("Saved file %v\n", filepath.Join(uploadDirectory, handler.Filename))
	}

	// Copy the uploaded file to the destination file
	_, err = file.Seek(0, io.SeekStart)
	if err {
		log.PrintLn("Faile to rewind file.")
	}
	_, err = io.Copy(adst, file)
	if err != nil {
		if debug {
			log.Println("Failed to save file")
			http.Error(w, "Failed to save file on server", http.StatusInternalServerError)
		}
		return
	}
	if debug {
		fmt.Printf("Saved archive file %v\n", filepath.Join(archiveDirectory, fileTime+handler.Filename))
	}

	ip := r.RemoteAddr
	log.Printf("File uploaded successfull from %v\n", ip)
}

func downloadHandler(w http.ResponseWriter, r *http.Request) {
	/*
		// Process a file request
		if r.Method != http.MethodGet {
			if debug {
				fmt.Println("Must use GET method")
				http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
			}
			return
		}
	*/
	// Open the file requested
	filePath := filepath.Join(uploadDirectory, r.URL.Path[len("/download/"):])
	file, err := os.Open(filePath)
	if err != nil {
		if debug {
			log.Printf("%v Not found\n", filePath)
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
		ip := r.RemoteAddr
		log.Printf("Failed to send file to %v\n", ip)
		http.Error(w, "Failed to send file", http.StatusInternalServerError)
		return
	}

	ip := r.RemoteAddr
	log.Printf("Sent %v to %v\n", filePath, ip)
}
