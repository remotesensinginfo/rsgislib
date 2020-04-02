#! /usr/bin/env python
############################################################################
#  notify_utils.py
#
#  Copyright 2020 RSGISLib.
#
#  RSGISLib: 'The remote sensing and GIS Software Library'
#
#  RSGISLib is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  RSGISLib is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
#
#
# Purpose: Provide a set of tools to notify users (e.g., of process
#          finishing).
#
# Author: Pete Bunting
# Email: petebunting@mac.com
# Date: 02/04/2020
# Version: 1.0
#
# History:
# Version 1.0 - Created.
#
###########################################################################


def send_email_notification(message, subject, receiver_email, sender_email=None, smtp_server=None, smtp_port=None,
                            smtp_password=None):
    """
    A function which sends an email.

    :param message: a string with the message to be sent.
    :param subject: a string with the subject of your email.
    :param receiver_email: the email address(es) of the receiver. If you want to email more than one person
                           then separate the email addresses with commas.
    :param sender_email: The email address of the sender. If None then will read variable from RSGIS_SENDER_EMAIL.
    :param smtp_server: The SMTP server to send the email. If None then will read variable from RSGIS_SMPT_SERVER.
    :param smtp_port: The SMTP server port. If None then will read variable from RSGIS_SMTP_PORT.
    :param smtp_password: The password for the SMTP server. If None then will read variable from RSGIS_PASSWORD.

    """
    import smtplib
    import ssl
    from email.mime.text import MIMEText
    from email.mime.multipart import MIMEMultipart
    import rsgislib
    rsgis_utils = rsgislib.RSGISPyUtils()

    if sender_email is None:
        sender_email = rsgis_utils.getEnvironmentVariable("RSGIS_SENDER_EMAIL")
        if sender_email is None:
            raise Exception("No senders email has been provided")

    if smtp_server is None:
        smtp_server = rsgis_utils.getEnvironmentVariable("RSGIS_SMPT_SERVER")
        if smtp_server is None:
            raise Exception("No SMTP server has been provided")

    if smtp_port is None:
        smtp_port = rsgis_utils.getEnvironmentVariable("RSGIS_SMTP_PORT")
        if smtp_port is None:
            raise Exception("No SMTP port has been provided")

    if smtp_password is None:
        smtp_password = rsgis_utils.getEnvironmentVariable("RSGIS_PASSWORD")
        if smtp_password is None:
            raise Exception("No SMTP password has been provided")

    email_message = MIMEMultipart()
    email_message["Subject"] = subject
    email_message["From"] = sender_email
    email_message["To"] = receiver_email
    email_message.attach(MIMEText(message))
    print(email_message.as_string())

    try:
        context = ssl.create_default_context()
        with smtplib.SMTP_SSL(smtp_server, smtp_port, context=context) as server:
            server.login(sender_email, smtp_password)
            server.sendmail(sender_email, receiver_email, email_message.as_string())
        print("Email has been send to {}".format(receiver_email))
    except smtplib.SMTPException as e:
        print("Error failed to send email: '{}'".format(e))
