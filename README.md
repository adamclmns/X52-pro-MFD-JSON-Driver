# X52-pro-MFD-JSON-Driver
Drive the Logitech/Saitech X52 pro MFD by writing to a JSON file.

This software will allow you to write to the MFD on the throttle base by updating the contents of a JSON file from a separate application. This means that you can use any programming language of your choice to determine what information to display, and won't have to deal with C++.

## Important Information
Grab the latest release version of this app. Unzip it to a location of your choice. Creating a separate folder and putting this app in it would be a good idea and also make for easy deletion if needed.

If the console window is clicked it will pause itself because it is waiting for input so, for it to continue functioning, hit the Esc key. To quit hit the 'X' button on the window or using 'Ctrl + C' in the console window.

Run the program with the path of the JSON file as command line input, and the pages and lines will be shown on the MFD of your HOTAS.

## Button press output

The tool will output the string `>MFD-SELECT PRESSED<` on standard-out when the MFD-select button on the HOTAS is pressed.

This can be used to react to button presses in client programs.


## JSON file format
The JSON file format is an object of the following structure 
```json
{
  "pages": [
    {
      "lines": [
        "Text line 1",
        "Text line 2",
        "..."
      ]
    },
    {
      "lines": [
        "Text line 1",
        "Text line 2",
        "..."
      ]
    },
    {
      "lines": [
       "Text line 1",
        "Text line 2",
        "..."
      ]
    }
  ]
}
```

The root object must have a `pages` property, which must be an array of "page" objects.

Each "page" object must have a `lines` property, which is an array of strings.

> **WARNING** The driver does not support changing the number of pages on the fly. The only way to add or remove pages is to restart the driver!

### Thanks
Frontier Developments for Elite Dangerous

Saitek for the use and development of the SDK to run this project

Niels Lohmann for work on [JSON for Modern C++](https://github.com/nlohmann/json)

[Anthony Zaprzalka](https://github.com/AZaps) for the initial version of this repository
 
