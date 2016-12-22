# SceneDocumentor
SceneDocumentor is - as the name indicates - a plugin for documenting your Cinema 4D scenes.

It allows you to add text information to any object in your scene, and also make it available for people who don't have SceneDocumentor installed. In addition to that, it can generate an exposé of your scene in HTML format. The style of the document can be styled by editing the template files and CSS stylesheet.

This plugin demonstrates the following C4D API aspects:
* Tag plugins, derived from `class TagData`
* Command plugins, derived from `class CommandData`
* Custom dockable dialogs, derived from `class GeDialog`
* Working with text files (reading from and writing to disk)
* Working with strings, replacing sub strings, string formatting, encoding, conversion
* Using dummy plugins as separator lines in the menu
