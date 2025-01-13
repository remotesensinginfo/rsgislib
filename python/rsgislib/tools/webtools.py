from typing import List


def find_web_imgs(base_url: str, file_ext: str = "jpg") -> List[str]:
    """
    A function which extracts all the URLs for the images with a specified
    file extension on a given web page. Note this will only return images
    which are specified using the <img> tag in the web page. Note this will
    only return image files which are on this page and does not follow links
    to other pages. Additionally, links which do not have the file extension
    specified will be ignored.

    :param base_url: A string representing the URL of the website
                     to search for images.
    :param file_ext: A string representing the file extension of the images
                     to search for. Defaults to "jpg".
    :return: A list of strings containing the URLs of images found on the website
             with the specified file extension.

    """
    import requests
    from bs4 import BeautifulSoup
    from urllib.parse import urljoin

    img_urls = list()
    try:
        # Send a GET request to the website
        response = requests.get(base_url)
        response.raise_for_status()  # Raise an HTTPError for bad responses

        # Parse the HTML content of the website
        soup = BeautifulSoup(response.text, "html.parser")

        # Find all <img> tags and extract URLs ending in specified URL
        for img_tag in soup.find_all("img"):
            img_url = img_tag.get("src")
            if img_url and img_url.lower().endswith(f".{file_ext}"):
                # Convert relative URLs to absolute URLs
                full_url = urljoin(base_url, img_url)
                img_urls.append(full_url)

    except requests.exceptions.RequestException as e:
        print(f"Error fetching website: {e}")
    return img_urls


def find_web_files(base_url: str, file_ext: str = "pdf") -> List[str]:
    """
    Get a list of file which are linked (using <a> tag) on the web page
    with the file extension specified. Note this will only return files
    which are on this page and does not follow links to other pages.
    Additionally, links which do not have the file extension specified
    will be ignored.

    :param base_url: The base URL of the website to search for files.
    :param file_ext: The file extension to filter for (default is "pdf").
    :return: A list of URLs for files with the specified extension found on the website.

    """
    import requests
    from bs4 import BeautifulSoup
    from urllib.parse import urljoin

    file_urls = list()
    try:
        # Send a GET request to the website
        response = requests.get(base_url)
        response.raise_for_status()  # Raise an HTTPError for bad responses

        # Parse the HTML content of the website
        soup = BeautifulSoup(response.text, "html.parser")

        # Find all <a> tags and extract URLs ending in specified URL
        for link in soup.find_all("a", href=True):
            href = link["href"]
            if href.lower().endswith(f".{file_ext}"):
                # Convert relative URLs to absolute URLs
                full_url = urljoin(base_url, href)
                file_urls.append(full_url)

    except requests.exceptions.RequestException as e:
        print(f"Error fetching website: {e}")
    return file_urls
