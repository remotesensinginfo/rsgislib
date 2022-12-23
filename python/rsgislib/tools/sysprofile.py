#!/usr/bin/env python
"""
The tools.sysprofile module contains functions which can be used to profile the memory and CPU usage of a system.
These are basic tools and do not replace more robust profiling tools.
"""


def create_mem_cpu_profile(out_file, interval=3, duration=600):
    """
    A function which creates a profile of memory and cpu usage of the system.

    This function provides information for the whole system not a single process. Therefore, if you are trying
    to study the performance of a single function you should minimise the use of the system for anything other
    than the experiment you are performing in-order to avoid misleading results.

    It is recommended that you run the function for a period (e.g., 30 seconds) before starting your processing
    so the background system usage can be estimated when the results are analysed.

    :param out_file: The output file to which the information will be saved.
    :param interval: The interval, in seconds, at which the information
                     should be recorded. (default: 3 seconds)
    :param duration: The time, in seconds, for which the function should
                     run for. (default: 600 seconds; 10 mins)

    """
    import time

    import psutil
    import tqdm

    n_steps = int(duration / interval)
    psutil.cpu_percent()
    time.sleep(1)
    print("Started logging...")
    with open(out_file, "w") as out_file_obj:
        start_time = time.time()
        out_file_obj.write("timestamp,interval,used_mem,cpu_percent\n")
        for i in tqdm.tqdm(range(n_steps)):
            current_time = time.time()
            current_time_inter = str(
                round(current_time - start_time, 1)
            )  # time in seconds
            mem_info = dict(psutil.virtual_memory()._asdict())
            used_mem = str(round(mem_info["used"] * 0.000000001, 2))  # mem usage in GB
            out_file_obj.write(
                "{}, {}, {}, {}\n".format(
                    current_time, current_time_inter, used_mem, psutil.cpu_percent()
                )
            )
            out_file_obj.flush()
            time.sleep(interval)


def plot_mem_cpu_profile(
    profile_file,
    out_plot_file="sys_profile.pdf",
    ref_period=30,
    interval=3,
    plot_format="PDF",
):
    """
    A function which creates a plot using the output of the rsgislib.tools.create_mem_cpu_profile function.
    A reference period can be used to estimate the background system usage which is removed from the whole
    timeseries.

    :param profile_file: a csv file from the rsgislib.tools.create_mem_cpu_profile function.
    :param out_plot_file: the output image file. Default: sys_profile.pdf
    :param ref_period: a reference period (in seconds) from which the background system resources will
                       be estimated and removed from the whole timeseries. Default: 30 seconds. If a value of
                       0 is provided (or less 4 x interval) then no reference period will be used.
    :param interval: the interval (in seconds) between measurements; used within the create_mem_cpu_profile function.
    :param plot_format: the file format for the outputted plot (i.e., PDF or PNG). Default: PDF

    """

    import matplotlib.pyplot as plt
    import pandas

    cols_dtypes = {
        "timestamp": float,
        "interval": float,
        "used_mem": float,
        "cpu_percent": float,
    }
    profile_df = pandas.read_csv(
        profile_file, delimiter=",", header=0, dtype=cols_dtypes
    )

    if (ref_period > 0) and (ref_period > (4 * interval)):
        ref_n_rows = int(ref_period / interval)
        avg_bkg_used_mem = profile_df[0:ref_n_rows].mean()["used_mem"]
        avg_bkg_cpu_percent = profile_df[0:ref_n_rows].mean()["cpu_percent"]

        profile_df["used_mem"] = profile_df["used_mem"] - avg_bkg_used_mem
        profile_df["cpu_percent"] = profile_df["cpu_percent"] - avg_bkg_cpu_percent
        profile_df["used_mem"][profile_df["used_mem"] < 0] = 0.0
        profile_df["cpu_percent"][profile_df["cpu_percent"] < 0] = 0.0

    time_max = profile_df["interval"].max()
    used_mem_max = profile_df["used_mem"].max()
    cpu_percent_max = profile_df["cpu_percent"].max()

    ax = profile_df.plot(x="interval", y="used_mem", color="r", label="Used Memory")
    profile_df.plot(
        x="interval",
        y="cpu_percent",
        color="b",
        label="Percent CPU",
        secondary_y=True,
        ax=ax,
    )

    ax.set_xlabel("time (seconds)")
    ax.set_xlim(0, time_max)
    ax.set_ylabel("Memory (Gb)")
    ax.set_ylim(0, used_mem_max * 1.10)
    ax.right_ax.set_ylabel("CPU Percent (%)")
    ax.right_ax.set_ylim(0, cpu_percent_max * 1.10)
    plt.savefig(out_plot_file, format=plot_format)
