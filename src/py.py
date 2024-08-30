import json

def read_and_format_json(file_path):
    try:
        # Open the JSON file
        with open(file_path, 'r') as file:
            data = json.load(file)
        
        # Format the output
        formatted_output = json.dumps(data, indent=4, sort_keys=True)
        
        return formatted_output
    
    except FileNotFoundError:
        print(f"File not found: {file_path}")
        return None
    except json.JSONDecodeError:
        print("Invalid JSON format")
        return None

# Usage example
json_file_path = "/home/hp/dev/Python/mal/anime_list.json" # "../data/5MB-min.json"
formatted_data = read_and_format_json(json_file_path)

if formatted_data:
    print(formatted_data)
