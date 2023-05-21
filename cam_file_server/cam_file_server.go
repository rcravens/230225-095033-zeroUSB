package main

import (
	"fmt"
	"io"
	"net/http"
	"os"
	"path/filepath"
)

// Form File - For security this should be a 32 char random number
const secureFormFile = "JrRdd728xJ5ewKT3Cg28dFYuwgbFXKU8"

// Make true for testing.
const debug = true

// Directory to server files from. 
const uploadDirectory = "/images/"

func main() {
        fmt.Println("Camera file server.");
        fmt.Println("Ready to receive file");
	fmt.Println("Listening on http://torks.org:8080")

	// HTTP Namespace
	http.HandleFunc("/upload", uploadHandler)
	http.HandleFunc("/download/", downloadHandler)

	http.ListenAndServe(":8080", nil)
}

func uploadHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		if ( debug ) {
			fmt.Println("Must use POST Method")
			http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		}
		return
	}

	file, handler, err := r.FormFile(secureFormFile)
	if err != nil {
		if ( debug ) {
			fmt.Println("Failed to receive file")
			http.Error(w, "Failed to receive file", http.StatusBadRequest)
		}
		return
	}
	defer file.Close()

	// Create the destination file
	dst, err := os.Create(filepath.Join(uploadDirectory, handler.Filename))
	if err != nil {
		if ( debug ) {
			fmt.Printf("Failed to create %v\n", filepath.Join(uploadDirectory, handler.Filename))
			http.Error(w, "Failed to create file on server", http.StatusInternalServerError)
		}
		return
	}
	defer dst.Close()

	// Copy the uploaded file to the destination file
	_, err = io.Copy(dst, file)
	if err != nil {
		if ( debug ) {
			fmt.Println("Failed to save file.")
			http.Error(w, "Failed to save file on server", http.StatusInternalServerError)
		}
		return
	}

	fmt.Printf("Saved file %v\n", filepath.Join(uploadDirectory, handler.Filename))

	if ( debug ) {
		fmt.Fprintf(w, "File uploaded successfully")
	}
}

func downloadHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodGet {
		if debug {
			fmt.Println("Must use GET method")
			http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		}
		return
	}

	filePath := filepath.Join(uploadDirectory, r.URL.Path[len("/download/"):])
	file, err := os.Open(filePath)
	if err != nil {
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
		fmt.Println("Failed to send file.")
		http.Error(w, "Failed to download file", http.StatusInternalServerError)
		return
	}

	if debug {
		fmt.Printf("Sent %v\n", filePath)
	}
}

