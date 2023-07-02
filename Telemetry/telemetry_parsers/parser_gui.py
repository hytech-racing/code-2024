import PySimpleGUI as sg
import time
import vector_parser_api as vp
import os
import scipy

if __name__ == "__main__":
    def process_folders(folder1, folder2):
        # Simulating a time-consuming process
        print("Processing folders:")
        print("Folder 1:", folder1)
        print("Folder 2:", folder2)
        start_t = time.time()
        data_dict, csv_paths = vp.parse_csv_folder(folder1)
        mat_paths = [os.path.splitext(csv_path)[0] + '.mat' for csv_path in csv_paths]
        if not (folder2 == ""):
            csv_path, mat_paths = vp.find_csv_dst_paths(folder1, folder2)
        for i in range(len(mat_paths)):
            scipy.io.savemat(mat_paths[i], {"data": data_dict[csv_paths[i]]})
        print(f"Parsed in {time.time() - start_t} seconds")
        # Add your code to process the selected folders here

    # Define the layout of the GUI
    layout = [
        [sg.Text("Folder 1:"), sg.Input(key="-FOLDER1-"), sg.FolderBrowse(target="-FOLDER1-")],
        [sg.Text("Folder 2:"), sg.Input(key="-FOLDER2-"), sg.FolderBrowse(target="-FOLDER2-")],
        [sg.Button("Process Folders", key="-BUTTON-", button_color=("white", "blue"), disabled=False)]
    ]

    # Create the window
    window = sg.Window("Folder Picker", layout)

    # Event loop to process events and button clicks
    while True:
        event, values = window.read()

        # Exit the program if the window is closed
        if event == sg.WINDOW_CLOSED:
            break

        if event == "-BUTTON-":
            folder1 = values["-FOLDER1-"]
            folder2 = values["-FOLDER2-"]

            # Disable the button and change its color and text
            window["-BUTTON-"].update(disabled=True, button_color=("white", "red"), text="Processing...")
            window.refresh()  # Update the window

            # Call the function and wait until it returns
            process_folders(folder1, folder2)

            # Enable the button and reset its color and text
            window["-BUTTON-"].update(disabled=False, button_color=("white", "blue"), text="Process Folders")
            window.refresh()  # Update the window

    # Close the window
    window.close()
